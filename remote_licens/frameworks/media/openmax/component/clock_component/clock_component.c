#define DBG_LEVEL               DBG_INFO
#define LOG_TAG                 "clock_component"

#include <base_component.h>
#include <unistd.h>
#include <lombo_dec_common.h>

#include "clock_component.h"
#include "clock_port.h"

/* #define AV_SYNC_LOG */

#ifdef AV_SYNC_LOG
static FILE *fd;
#define FLOG(fmt, ...) fprintf(fd, fmt, ##__VA_ARGS__)
#else
#define FLOG(fmt, ...)
#endif

#define DEFAULT_THRESH 2000 /* TODO: what is a good thresh to use */

/** Clock component private structure.
 * see the define above
 * @param clock_state This structure holds the state of the clock
 * @param sem_starttime the semaphore that coordinates the arrival
 *	of start times from all clients
 * @param sem_clockevent the semaphore that coordinates clock event
 *	received from the client
 * @param sem_clkevent_complete the semaphore that coordinates clock
 *	event sent to the client
 * @param walltime_base the wall time at which the clock was started
 * @param mediatime_base the Media time at which the clock was started
 * @param update_type indicates the type of update received
 *	from the clock src component
 * @param min_starttime keeps the minimum starttime of the clients
 * @param config_scale Representing the current media time scale factor
 */
typedef struct clock_component_private {
	OMX_TIME_CONFIG_CLOCKSTATETYPE      clock_state;
	OMX_TIME_CONFIG_ACTIVEREFCLOCKTYPE  ref_clock;
	sem_t                               sem_starttime;
	sem_t                               sem_clockevent;
	sem_t                               sem_clkevent_complete;
	OMX_TICKS                           walltime_base;
	OMX_TICKS                           mediatime_base;
	OMX_TICKS                           mediatime;
	OMX_TIME_UPDATETYPE                 update_type;
	OMX_TIME_CONFIG_TIMESTAMPTYPE       min_starttime;
	OMX_TIME_CONFIG_SCALETYPE           config_scale;

	OMX_TIME_CONFIG_ACTIVEREFCLOCKTYPE  req_ref_clock;
	pthread_t refclk_thd;
	int refclk_thd_run;
	pthread_cond_t aref_cond;
	pthread_mutex_t aref_lock;
} clock_component_private_t;

static inline void _set_ref_clock(clock_component_private_t *priv,
	OMX_TIME_REFCLOCKTYPE eclock)
{
	priv->ref_clock.eClock = eclock;
	if (priv->clock_state.eState == OMX_TIME_ClockStateRunning) {
		priv->mediatime_base = priv->mediatime;
		priv->walltime_base = oscl_get_usec();
	}
}

static void *ref_clock_manager(void *param)
{
	lb_omx_component_t *cmp_handle = param;
	clock_component_private_t *priv = cmp_handle->component_private;
	int ret;

	while (priv->refclk_thd_run) {
		pthread_mutex_lock(&priv->aref_lock);
		ret = oscl_cond_timedwait_s(&priv->aref_cond, &priv->aref_lock, 1);
		pthread_mutex_unlock(&priv->aref_lock);
		if (cmp_handle->state != OMX_StateExecuting)
			continue;
		if (ETIMEDOUT == ret) {
			OSCL_LOGD("wait for audio reference 1000 ms timeout");
			if (OMX_TIME_RefClockAudio == priv->ref_clock.eClock) {
				OSCL_LOGD("switch refclock to none");
				_set_ref_clock(priv, OMX_TIME_RefClockNone);
			}
		} else {
			if (OMX_TIME_RefClockNone == priv->ref_clock.eClock &&
				OMX_TIME_RefClockAudio == priv->req_ref_clock.eClock) {
				OSCL_LOGD("switch refclock to audio");
				_set_ref_clock(priv, OMX_TIME_RefClockAudio);
			}
		}
	}

	OSCL_LOGD("exit");
	return NULL;
}

static int ref_clock_init(lb_omx_component_t *cmp_handle)
{
	clock_component_private_t *priv = cmp_handle->component_private;
	pthread_condattr_t condattr;
	int ret;

	pthread_condattr_init(&condattr);
	pthread_condattr_setclock(&condattr, CLOCK_MONOTONIC);
	pthread_cond_init(&priv->aref_cond, &condattr);
	pthread_mutex_init(&priv->aref_lock, NULL);

	priv->refclk_thd_run = 1;
	ret = pthread_create(&priv->refclk_thd, NULL, ref_clock_manager, cmp_handle);
	if (ret != 0) {
		OSCL_LOGE(" create ref_clock_manager error!");
		return -1;
	}

	return 0;
}

static void ref_clock_deinit(lb_omx_component_t *cmp_handle)
{
	clock_component_private_t *priv = cmp_handle->component_private;

	OSCL_LOGD("joining ref_clock_manager");
	priv->refclk_thd_run = 0;
	pthread_mutex_lock(&priv->aref_lock);
	pthread_cond_signal(&priv->aref_cond);
	pthread_mutex_unlock(&priv->aref_lock);
	pthread_join(priv->refclk_thd, NULL);
	OSCL_LOGD("join ref_clock_manager end");

	pthread_mutex_destroy(&priv->aref_lock);
	pthread_cond_destroy(&priv->aref_cond);
}

OMX_ERRORTYPE clock_get_config(OMX_IN OMX_HANDLETYPE cmp_hdl,
	OMX_IN OMX_INDEXTYPE index,
	OMX_INOUT OMX_PTR data)
{
	lb_omx_component_t *component = NULL;
	clock_component_private_t *cmp_priv = NULL;
	OMX_TIME_CONFIG_CLOCKSTATETYPE     *clockstate;
	OMX_TIME_CONFIG_TIMESTAMPTYPE      *timestamp;
	OMX_TIME_CONFIG_SCALETYPE           *config_scale;
	OMX_TIME_CONFIG_ACTIVEREFCLOCKTYPE  *refclk;
	OMX_S32                             scale;
	OMX_TICKS                           walltime;

	oscl_param_check(cmp_hdl != NULL, OMX_ErrorBadParameter, NULL);
	component = get_lb_component(cmp_hdl);
	cmp_priv = (clock_component_private_t *)component->component_private;
	scale = cmp_priv->config_scale.xScale >> 16;

	switch (index) {
	case OMX_IndexConfigTimeClockState:
		clockstate = (OMX_TIME_CONFIG_CLOCKSTATETYPE *)data;
		memcpy(clockstate, &cmp_priv->clock_state, sizeof(*clockstate));
		break;
	case OMX_IndexConfigTimeCurrentWallTime:
		timestamp = (OMX_TIME_CONFIG_TIMESTAMPTYPE *)data;
		timestamp->nTimestamp = oscl_get_usec();
		break;
	case OMX_IndexConfigTimeCurrentMediaTime:
		timestamp = (OMX_TIME_CONFIG_TIMESTAMPTYPE *)data;
		if (cmp_priv->ref_clock.eClock == OMX_TIME_RefClockNone) {
			walltime = oscl_get_usec();
			timestamp->nTimestamp = cmp_priv->mediatime_base +
				scale * (walltime - cmp_priv->walltime_base);
			cmp_priv->mediatime = timestamp->nTimestamp;
			OSCL_LOGD("walltime %lld\n", walltime);
		} else {
			timestamp->nTimestamp = cmp_priv->mediatime;
			OSCL_LOGD("mediatime %lld\n", cmp_priv->mediatime);
		}
		break;
	case OMX_IndexConfigTimeScale:
		config_scale = (OMX_TIME_CONFIG_SCALETYPE *)data;
		memcpy(config_scale, &cmp_priv->config_scale, sizeof(*config_scale));
		break;
	case OMX_IndexConfigTimeActiveRefClock:
		refclk = (OMX_TIME_CONFIG_ACTIVEREFCLOCKTYPE *)data;
		memcpy(refclk, &cmp_priv->req_ref_clock, sizeof(*refclk));
		break;
	default:
		OSCL_LOGE("error omx config index %d\n", index);
		return OMX_ErrorBadParameter;
		break;
	}
	return OMX_ErrorNone;
}

static void config_mediatime_request(lb_omx_component_t *component,
	OMX_IN OMX_PTR data)
{
	clock_component_private_t *cmp_priv = NULL;
	clock_port_private_t *port_priv = NULL;
	OMX_TIME_CONFIG_MEDIATIMEREQUESTTYPE *mediatime_request;
	OMX_U32                             portIndex;
	OMX_TICKS walltime, mediatime, mediatime_diff, walltime_diff;
	OMX_S32                             scale;
	unsigned int                        sleeptime;

	cmp_priv = (clock_component_private_t *)component->component_private;
	scale = cmp_priv->config_scale.xScale >> 16;

	if (cmp_priv->clock_state.eState != OMX_TIME_ClockStateStopped &&
		scale != 0) { /* todo: what happens if call in pause mode */
		/* update the mediatime request */
		mediatime_request = (OMX_TIME_CONFIG_MEDIATIMEREQUESTTYPE *)data;
		portIndex = mediatime_request->nPortIndex;
		port_priv =
			(clock_port_private_t *)component->port[portIndex].port_private;
		memcpy(&port_priv->mediatime_request,
			mediatime_request, sizeof(*mediatime_request));

		/* cal the cur mediatime and diff from request */
		walltime = oscl_get_usec();
		mediatime = cmp_priv->mediatime_base +
			scale * (walltime - cmp_priv->walltime_base);
		mediatime_diff = mediatime_request->nMediaTimestamp -
			mediatime_request->nOffset * scale - mediatime;
		FLOG("portindex %ld wt %lld mt %lld mtdiff %lld, request %lld\n",
			portIndex, walltime, mediatime, mediatime_diff,
			mediatime_request->nMediaTimestamp);
		if ((mediatime_diff < 0 && scale > 0) ||
			(mediatime_diff > 0 && scale < 0)) {
			/* if mediatime has already elapsed
			 * then request can not be fullfilled */
			port_priv->mediatime.eUpdateType =
				OMX_TIME_UpdateRequestFulfillment;
			port_priv->mediatime.nMediaTimestamp =
				mediatime_request->nMediaTimestamp;
			port_priv->mediatime.nOffset = -1;
		} else {
			walltime_diff = mediatime_diff / scale;
			if (mediatime_diff) {
				if (walltime_diff > DEFAULT_THRESH) {
					sleeptime = (unsigned int)(walltime_diff -
							DEFAULT_THRESH)/1000;
					oscl_mdelay(sleeptime);
				/* todo: make sure if we can use this as new diff */
					walltime_diff = DEFAULT_THRESH;
					walltime = oscl_get_usec();
					mediatime = cmp_priv->mediatime_base +
						scale * (walltime -
							cmp_priv->walltime_base);
				}
				port_priv->mediatime.nMediaTimestamp =
					mediatime_request->nMediaTimestamp;
				port_priv->mediatime.nWallTimeAtMediaTime =
					walltime + walltime_diff;
				port_priv->mediatime.nOffset = walltime_diff;
				port_priv->mediatime.xScale =
					cmp_priv->config_scale.xScale;
				port_priv->mediatime.eUpdateType =
					OMX_TIME_UpdateRequestFulfillment;

			}
		}
		/*Signal Buffer Management Thread*/
		sem_post(component->buf_mgnt_sem);
		/*sem_post(&cmp_priv->sem_clockevent);*/
		OSCL_LOGD("Waiting for fullfillment handled\n");
		sem_wait(&cmp_priv->sem_clkevent_complete);
	}
}

OMX_ERRORTYPE clock_set_config(OMX_IN OMX_HANDLETYPE cmp_hdl,
	OMX_IN OMX_INDEXTYPE index,
	OMX_IN OMX_PTR data)
{
	lb_omx_component_t *component = NULL;
	clock_component_private_t *cmp_priv = NULL;
	clock_port_private_t *port_priv = NULL;
	OMX_TIME_CONFIG_CLOCKSTATETYPE      *clockstate;
	OMX_TIME_CONFIG_TIMESTAMPTYPE       *ref_timestamp;
	OMX_TIME_CONFIG_ACTIVEREFCLOCKTYPE  *ref_clock;
	OMX_U32                             portIndex;
	base_port_t                         *port;
	OMX_TIME_CONFIG_SCALETYPE           *config_scale;
	OMX_U32                             mask;
	int                                 i;
	OMX_TICKS                           walltime;
	/* OMX_S32                             scale; */

	oscl_param_check(cmp_hdl != NULL, OMX_ErrorBadParameter, NULL);
	component = get_lb_component(cmp_hdl);
	cmp_priv = component->component_private;

	switch (index) {
	case OMX_IndexConfigTimeClockState:
		clockstate = (OMX_TIME_CONFIG_CLOCKSTATETYPE *)data;
		switch (clockstate->eState) {
		case OMX_TIME_ClockStateRunning:
			if (cmp_priv->clock_state.eState == OMX_TIME_ClockStateRunning)
				OSCL_LOGI("receive OMX_TIME_ClockStateRunning again\n");
			memcpy(&cmp_priv->clock_state, clockstate, sizeof(*clockstate));
			cmp_priv->update_type = OMX_TIME_UpdateClockStateChanged;
			/* update state change in all port */
			for (i = 0; i < component->num_ports; i++) {
				port_priv = (clock_port_private_t *)
						component->port[i].port_private;
				port_priv->mediatime.eUpdateType =
					OMX_TIME_UpdateClockStateChanged;
				port_priv->mediatime.eState = OMX_TIME_ClockStateRunning;
				port_priv->mediatime.xScale =
					cmp_priv->config_scale.xScale;
			}
			/* Signal Buffer Management Thread */
			/*sem_post(&cmp_priv->sem_clockevent);*/
			sem_post(component->buf_mgnt_sem);
			OSCL_LOGD("Waiting for Clock Running Event for all ports\n");
			sem_wait(&cmp_priv->sem_clkevent_complete);
			break;
		case OMX_TIME_ClockStateWaitingForStartTime:
			if (cmp_priv->clock_state.eState == OMX_TIME_ClockStateRunning) {
				OSCL_LOGE("incorrect state transition\n");
				return OMX_ErrorIncorrectStateTransition;
			} else if (cmp_priv->clock_state.eState ==
				OMX_TIME_ClockStateWaitingForStartTime) {
				OSCL_LOGE("error same state\n");
				return OMX_ErrorSameState;
			}
			OSCL_LOGI("set to state waiting for starttime\n");
			memcpy(&cmp_priv->clock_state, clockstate, sizeof(*clockstate));
			cmp_priv->min_starttime.nTimestamp = -1;
			break;
		case OMX_TIME_ClockStateStopped:
			memcpy(&cmp_priv->clock_state, clockstate, sizeof(*clockstate));
			cmp_priv->update_type = OMX_TIME_UpdateClockStateChanged;
			/* update state change in all port */
			for (i = 0; i < component->num_ports; i++) {
				port_priv = (clock_port_private_t *)
					component->port[i].port_private;
				port_priv->mediatime.eUpdateType =
					OMX_TIME_UpdateClockStateChanged;
				port_priv->mediatime.eState = OMX_TIME_ClockStateStopped;
				port_priv->mediatime.xScale =
					cmp_priv->config_scale.xScale;
			}
			/* Signal Buffer Management Thread */
			/*sem_post(&cmp_priv->sem_clockevent);*/
			sem_post(component->buf_mgnt_sem);
			OSCL_LOGD("Waiting for Clock Running Event for all ports\n");
			sem_wait(&cmp_priv->sem_clkevent_complete);
			OSCL_LOGD("Waiting clock running event complete\n");
			break;
		default:
			OSCL_LOGE("error clock state!\n");
			break;
		}
		break;
	case OMX_IndexConfigTimeClientStartTime:
		ref_timestamp = (OMX_TIME_CONFIG_TIMESTAMPTYPE *)data;
		portIndex = ref_timestamp->nPortIndex;
		if (portIndex > component->num_ports) {
			OSCL_LOGE("error port index %d\n", portIndex);
			return OMX_ErrorBadPortIndex;
		}
		port = &component->port[portIndex];
		if (!PORT_IS_ENABLED(port)) {
			OSCL_LOGE("port %d is disabled\n", portIndex);
			return OMX_ErrorBadParameter;
		}
		port_priv = (clock_port_private_t *)port->port_private;
		memcpy(&port_priv->timestamp, ref_timestamp, sizeof(*ref_timestamp));
		/* it's the first client to send the start time,
		 * so init the min_starttime */
		if ((cmp_priv->min_starttime.nTimestamp == -1)  &&
			(portIndex < CLOCK_PORT_SUB))
			cmp_priv->min_starttime.nTimestamp =
				port_priv->timestamp.nTimestamp;

		/* update the nWaitMask to clear the flag for the client
		 * which has sent its start time */
		if (cmp_priv->clock_state.nWaitMask) {
			mask = ~(0x1 << portIndex);
			cmp_priv->clock_state.nWaitMask &= mask;
			if ((cmp_priv->min_starttime.nTimestamp >=
				port_priv->timestamp.nTimestamp) &&
				(portIndex < CLOCK_PORT_SUB)) {
				cmp_priv->min_starttime.nTimestamp =
					port_priv->timestamp.nTimestamp;
				cmp_priv->min_starttime.nPortIndex =
					port_priv->timestamp.nPortIndex;
			}
		}
		OSCL_LOGI("WaitMask=0x%08x, eState=%d", cmp_priv->clock_state.nWaitMask,
			cmp_priv->clock_state.eState);
		/* if there is no port to wait for, then set clock state to running */
		if (!cmp_priv->clock_state.nWaitMask &&
			cmp_priv->clock_state.eState ==
				OMX_TIME_ClockStateWaitingForStartTime) {
			OSCL_LOGI("%s config starttime %lld",
				((portIndex == CLOCK_PORT_AUDIO) ? "audio" : "video"),
				port_priv->timestamp.nTimestamp);
			cmp_priv->clock_state.eState = OMX_TIME_ClockStateRunning;
			cmp_priv->clock_state.nStartTime =
				cmp_priv->min_starttime.nTimestamp;
			cmp_priv->mediatime_base = cmp_priv->min_starttime.nTimestamp;
			cmp_priv->mediatime = cmp_priv->min_starttime.nTimestamp;
			walltime = oscl_get_usec();
			cmp_priv->walltime_base = walltime;
			OSCL_LOGI("mediatime base %lld, walltime base %lld",
				cmp_priv->mediatime_base, cmp_priv->walltime_base);
			cmp_priv->update_type = OMX_TIME_UpdateClockStateChanged;
			/* update state change in all port */
			for (i = 0; i < component->num_ports; i++) {
				port_priv = (clock_port_private_t *)
					component->port[i].port_private;
				port_priv->mediatime.eUpdateType =
					OMX_TIME_UpdateClockStateChanged;
				port_priv->mediatime.eState = OMX_TIME_ClockStateRunning;
				port_priv->mediatime.xScale =
					cmp_priv->config_scale.xScale;
			}
			/* Signal Buffer Management Thread */
			/*sem_post(&cmp_priv->sem_clockevent);*/
			sem_post(component->buf_mgnt_sem);
			OSCL_LOGI("Waiting for Clock Running Event for all ports\n");
			sem_wait(&cmp_priv->sem_clkevent_complete);
			OSCL_LOGI("Waiting clock running event complete\n");
		}
		if (CLOCK_PORT_AUDIO == portIndex &&
			OMX_TIME_RefClockAudio == cmp_priv->req_ref_clock.eClock) {
			OSCL_LOGI("set refclock to audio");
			_set_ref_clock(cmp_priv, OMX_TIME_RefClockAudio);
		}
		break;
	case OMX_IndexConfigTimeActiveRefClock:
		ref_clock = (OMX_TIME_CONFIG_ACTIVEREFCLOCKTYPE *)data;
		memcpy(&cmp_priv->req_ref_clock, ref_clock, sizeof(*ref_clock));
		break;
	case OMX_IndexConfigTimeCurrentAudioReference:
		/* update ref clock from audio ref clock */
		ref_timestamp = (OMX_TIME_CONFIG_TIMESTAMPTYPE *)data;
		portIndex = ref_timestamp->nPortIndex;
		if (portIndex > component->num_ports) {
			OSCL_LOGE("error port index %d\n", portIndex);
			return OMX_ErrorBadPortIndex;
		}
		port_priv = (clock_port_private_t *)
			component->port[portIndex].port_private;
		memcpy(&port_priv->timestamp, ref_timestamp, sizeof(*ref_timestamp));
		if (ref_timestamp->nTimestamp == 0xffffffff) {
			OSCL_LOGI("audio clock end!\n");
			_set_ref_clock(cmp_priv, OMX_TIME_RefClockNone);
			return OMX_ErrorNone;
		}
		if (cmp_priv->ref_clock.eClock != OMX_TIME_RefClockAudio &&
			cmp_priv->req_ref_clock.eClock == OMX_TIME_RefClockAudio) {
			OSCL_LOGI("set refclock to audio");
			_set_ref_clock(cmp_priv, OMX_TIME_RefClockAudio);
		}
		OSCL_LOGD("config audio ref ts %lld", ref_timestamp->nTimestamp);
		/* only update time base if refclk is audio */
		if (cmp_priv->ref_clock.eClock == OMX_TIME_RefClockAudio)
			cmp_priv->mediatime = ref_timestamp->nTimestamp;
		pthread_mutex_lock(&cmp_priv->aref_lock);
		pthread_cond_signal(&cmp_priv->aref_cond);
		pthread_mutex_unlock(&cmp_priv->aref_lock);
		break;
	case OMX_IndexConfigTimeCurrentVideoReference:
		/* update ref clock from video ref clock */
		ref_timestamp = (OMX_TIME_CONFIG_TIMESTAMPTYPE *)data;
		portIndex = ref_timestamp->nPortIndex;
		if (portIndex > component->num_ports) {
			OSCL_LOGE("error port index %d\n", portIndex);
			return OMX_ErrorBadPortIndex;
		}
		port_priv = (clock_port_private_t *)
			component->port[portIndex].port_private;
		memcpy(&port_priv->timestamp, ref_timestamp, sizeof(*ref_timestamp));
		if (cmp_priv->ref_clock.eClock != OMX_TIME_RefClockVideo &&
			cmp_priv->req_ref_clock.eClock == OMX_TIME_RefClockVideo) {
			OSCL_LOGI("set refclock to video");
			_set_ref_clock(cmp_priv, OMX_TIME_RefClockVideo);
		}
		/* only update time base if refclk is video */
		OSCL_LOGD("config video ref ts %lld", ref_timestamp->nTimestamp);
		if (cmp_priv->ref_clock.eClock == OMX_TIME_RefClockVideo)
			cmp_priv->mediatime = ref_timestamp->nTimestamp;
		break;
	case OMX_IndexConfigTimeScale:
		/* update the mediatime base and walltime base
		 * using the current scale value*/
		walltime = oscl_get_usec();
		cmp_priv->walltime_base = walltime;
		cmp_priv->mediatime_base = cmp_priv->mediatime;

		/* update the new scale value */
		config_scale = (OMX_TIME_CONFIG_SCALETYPE *)data;
		memcpy(&cmp_priv->config_scale, config_scale, sizeof(*config_scale));
		cmp_priv->update_type = OMX_TIME_UpdateScaleChanged;

		/* update the scale change in all ports */
		for (i = 0; i < component->num_ports; i++) {
			port_priv = (clock_port_private_t *)
				component->port[i].port_private;
			port_priv->mediatime.eUpdateType = OMX_TIME_UpdateScaleChanged;
			port_priv->mediatime.eState = OMX_TIME_ClockStateRunning;
			port_priv->mediatime.xScale = cmp_priv->config_scale.xScale;
			port_priv->mediatime.nMediaTimestamp = cmp_priv->mediatime;
			port_priv->mediatime.nWallTimeAtMediaTime = walltime;
		}
		/* Signal Buffer Management Thread */
		/*sem_post(&cmp_priv->sem_clockevent);*/
		sem_post(component->buf_mgnt_sem);
		OSCL_LOGD("Waiting for scale change handled for all ports\n");
		sem_wait(&cmp_priv->sem_clkevent_complete);
		OSCL_LOGD("time scale change to %d\n", config_scale->xScale);
		break;
	case OMX_IndexConfigTimeMediaTimeRequest:
		config_mediatime_request(component, data);
		break;
	default:
		OSCL_LOGE("error config index %d\n", index);
		return OMX_ErrorBadParameter;
		break;
	}
	return OMX_ErrorNone;
}

OMX_ERRORTYPE clock_get_parameter(OMX_IN OMX_HANDLETYPE cmp_hdl,
	OMX_IN OMX_INDEXTYPE index,
	OMX_INOUT OMX_PTR param_data)
{
	lb_omx_component_t *component = NULL;
	OMX_OTHER_PARAM_PORTFORMATTYPE *port_format;
	clock_port_private_t *port_priv;

	oscl_param_check(cmp_hdl != NULL, OMX_ErrorBadParameter, NULL);
	component = get_lb_component(cmp_hdl);

	switch (index) {
	case OMX_IndexParamOtherPortFormat:
		port_format = (OMX_OTHER_PARAM_PORTFORMATTYPE *)param_data;
		if (port_format->nIndex < component->num_ports) {
			port_priv = (clock_port_private_t *)
				component->port[port_format->nIndex].port_private;
			memcpy(port_format, &port_priv->other_param,
				sizeof(*port_format));
		} else {
			OSCL_LOGE("bad port index %d\n", port_format->nIndex);
			return OMX_ErrorBadPortIndex;
		}
		break;
	default:
		return base_get_parameter(cmp_hdl, index, param_data);
		break;
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE clock_set_parameter(OMX_IN OMX_HANDLETYPE cmp_hdl,
	OMX_IN OMX_INDEXTYPE index,
	OMX_IN OMX_PTR param_data)
{
	lb_omx_component_t *component = NULL;
	OMX_OTHER_PARAM_PORTFORMATTYPE *port_format;
	clock_port_private_t *port_priv;

	oscl_param_check(cmp_hdl != NULL, OMX_ErrorBadParameter, NULL);
	component = get_lb_component(cmp_hdl);

	switch (index) {
	case OMX_IndexParamOtherPortFormat:
		port_format = (OMX_OTHER_PARAM_PORTFORMATTYPE *)param_data;
		if (port_format->nIndex < component->num_ports) {
			port_priv = (clock_port_private_t *)
				component->port[port_format->nIndex].port_private;
			memcpy(&port_priv->other_param,
				port_format, sizeof(*port_format));
		} else {
			OSCL_LOGE("bad port index %d\n", port_format->nIndex);
			return OMX_ErrorBadPortIndex;
		}
		break;
	default:
		return base_set_parameter(cmp_hdl, index, param_data);
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE clock_set_state(OMX_HANDLETYPE hComp,
	OMX_U32 dest_state)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	lb_omx_component_t *component = NULL;
	clock_component_private_t *cmp_priv = NULL;

	oscl_param_check(hComp != NULL, OMX_ErrorBadParameter, NULL);
	component = get_lb_component(hComp);
	cmp_priv = component->component_private;

	OSCL_LOGI("%s->%s", omx_state_as_string(component->state),
		omx_state_as_string(dest_state));

	if (component->state == OMX_StateIdle &&
		dest_state == OMX_StateLoaded) {
		OSCL_LOGI("signal clock component end\n");
		/*sem_post(&cmp_priv->sem_clockevent);*/
		sem_post(component->buf_mgnt_sem);
#ifdef AV_SYNC_LOG
		fclose(fd);
#endif
	}

	if (component->state == OMX_StateIdle &&
		dest_state == OMX_StateExecuting) {
		cmp_priv->mediatime = 0;
		cmp_priv->mediatime_base = 0;
		cmp_priv->walltime_base = 0;
	}

	if ((component->state == OMX_StateExecuting ||
		component->state == OMX_StatePause) &&
		dest_state == OMX_StateIdle) {
		cmp_priv->clock_state.eState = OMX_TIME_ClockStateStopped;
		cmp_priv->ref_clock.eClock = OMX_TIME_RefClockNone;
		cmp_priv->req_ref_clock.eClock = OMX_TIME_RefClockNone;
		sem_post(component->mgmt_flush_sem);
	}

	if (component->state == OMX_StateLoaded &&
		dest_state == OMX_StateIdle) {
		cmp_priv->clock_state.eState = OMX_TIME_ClockStateStopped;
#ifdef AV_SYNC_LOG
		fd = fopen("clock_timestamps.log", "w");
#endif
	}

	if (component->state == OMX_StatePause && dest_state == OMX_StateExecuting) {
		cmp_priv->mediatime_base = cmp_priv->mediatime;
		cmp_priv->walltime_base = oscl_get_usec();
	}

	if (dest_state == OMX_StateExecuting) {
		pthread_mutex_lock(&cmp_priv->aref_lock);
		pthread_cond_signal(&cmp_priv->aref_cond);
		pthread_mutex_unlock(&cmp_priv->aref_lock);
	}

	ret = base_component_set_state(hComp, dest_state);

	return ret;
}


void clock_buffer_handle(OMX_HANDLETYPE cmp_hdl,
	OMX_BUFFERHEADERTYPE *inbuffer,
	OMX_BUFFERHEADERTYPE *outbuffer)
{
	lb_omx_component_t *component = NULL;
	clock_port_private_t *port_priv;

	component = get_lb_component(cmp_hdl);
	port_priv = (clock_port_private_t *)
		component->port[outbuffer->nOutputPortIndex].port_private;

	memcpy(outbuffer->pBuffer, &port_priv->mediatime,
		sizeof(OMX_TIME_MEDIATIMETYPE));
	outbuffer->nFilledLen = sizeof(OMX_TIME_MEDIATIMETYPE);
	/* clear the update type */
	port_priv->mediatime.eUpdateType = OMX_TIME_UpdateMax;
}


void *clock_buffer_manager(void *param)
{
	lb_omx_component_t *component = NULL;
	clock_component_private_t *cmp_priv = NULL;
	OMX_BUFFERHEADERTYPE *output_buf[MAX_CLOCK_PORTS + DMX_SUBTITLE_STREAM_NUM];
	int                  being_flushed = 0;
	int                  i;

	oscl_param_check((param != NULL), NULL, "null params");
	component = get_lb_component(param);
	cmp_priv = (clock_component_private_t *)component->component_private;
	for (i = 0; i < component->num_ports; i++)
		output_buf[i] = NULL;

	while (component->state == OMX_StateIdle
		|| component->state == OMX_StateExecuting
		|| component->state == OMX_StatePause
		|| component->target_state == OMX_StateIdle) {

		/* flush the ports if they are being flushed */
		pthread_mutex_lock(&component->flush_mutex);
		for (i = 0; i < component->num_ports; i++)
			being_flushed |= component->port[i].is_flushed;
		while (being_flushed) {
			pthread_mutex_unlock(&component->flush_mutex);
			for (i = 0; i < component->num_ports; i++) {
				base_port_t *port = &component->port[i];
				if (!PORT_IS_ENABLED(port))
					continue;
				if (output_buf[i] && port->is_flushed) {
					port->return_buffer(port, output_buf[i]);
					output_buf[i] = NULL;
					OSCL_LOGI("port %d being flush\n", i);
				}
			}
			OSCL_LOGI("wait flush_sem");
			sem_post(component->mgmt_flush_sem);
			sem_wait(component->flush_sem);
			OSCL_LOGI("wait flush_sem end");
			pthread_mutex_lock(&component->flush_mutex);

			being_flushed = 0;
		}
		pthread_mutex_unlock(&component->flush_mutex);

		/* wait until the state change to executing */
		if (component->state != OMX_StateExecuting) {
			sem_wait(component->buf_mgnt_sem);
			continue;
		}
		sem_wait(component->buf_mgnt_sem);
#if 0
		OSCL_LOGD("waiting for clock event\n");
		sem_wait(&cmp_priv->sem_clockevent);
		OSCL_LOGD("clock event occured sem value:%d",
			sem_get(&cmp_priv->sem_clockevent));
#endif

		/* todo: maybe we need to do something for unturnnel mode */

		for (i = 0; i < component->num_ports; i++) {
			base_port_t *port = &component->port[i];
			clock_port_private_t *priv = port->port_private;
			OMX_BUFFERHEADERTYPE *buf;

			if (!PORT_IS_ENABLED(port)) {
				OSCL_LOGD("port %d disable, continue\n", i);
				continue;
			}
			if (component->state == OMX_StateLoaded ||
				component->state == OMX_StateInvalid) {
				OSCL_LOGW("state %d, break\n", component->state);
				break;
			}
			if (priv->mediatime.eUpdateType ==
				OMX_TIME_UpdateClockStateChanged ||
				priv->mediatime.eUpdateType ==
				OMX_TIME_UpdateScaleChanged ||
				priv->mediatime.eUpdateType ==
				OMX_TIME_UpdateRequestFulfillment) {
				int ret;

				if (sem_get(&port->buffer_sem) <= 0) {
					OSCL_LOGE("have clk event %d, wait for buffer\n",
						priv->mediatime.eUpdateType);
				}

				OSCL_LOGD("port %d output_sem value %d",
					i, sem_get(&port->buffer_sem));
				ret = oscl_sem_timedwait_ms(&port->buffer_sem, 3000);
				if (ret == -1) {
					OSCL_LOGE("port %d buffer_sem wait err(%s)!",
						i, strerror(errno));
				}
				buf = oscl_queue_dequeue(&port->buffer_queue);
				OSCL_LOGD("port %d after wait buffer_sem=%d,buf=%p",
					i, sem_get(&port->buffer_sem), buf);

				/* process output buffer on port i */
				if (buf != NULL) {
					if (component->buf_handle)
						component->buf_handle(component, NULL,
							buf);
					else
						buf->nFilledLen = 0;
				}
				/* if buffer has been produced, then return it */
				if (buf && buf->nFilledLen != 0) {
					OMX_ERRORTYPE res;
					res = port->return_buffer(port, buf);
					if (res != OMX_ErrorNone)
						OSCL_LOGW("port %d return buf fail!", i);
					buf = NULL;
				}
				output_buf[i] = buf;
			}
		}
		/* sent clock event for all port */
		sem_post(&cmp_priv->sem_clkevent_complete);
	}
	OSCL_LOGI("end of clock buffer manager thread!\n");
	return NULL;
}


OMX_ERRORTYPE clock_component_deinit(OMX_IN OMX_HANDLETYPE hComponent)
{
	OMX_U32 i;
	lb_omx_component_t *component = NULL;
	clock_component_private_t *private = NULL;

	oscl_param_check(hComponent != NULL, OMX_ErrorBadParameter, NULL);
	component = get_lb_component(hComponent);

	ref_clock_deinit(component);
	/* deinit clock port */
	for (i = 0; i < component->num_ports; i++)
		clock_port_deinit(&component->port[i]);

	private = component->component_private;
	sem_destroy(&private->sem_starttime);
	sem_destroy(&private->sem_clockevent);
	sem_destroy(&private->sem_clkevent_complete);

	oscl_free(component->component_private);
	base_component_deinit(hComponent);
	return OMX_ErrorNone;
}

OMX_ERRORTYPE clock_component_init(lb_omx_component_t *cmp_handle)
{
	OMX_U32 i, j;
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	clock_component_private_t *private = NULL;

	private = oscl_zalloc(sizeof(*private));
	if (!private) {
		OSCL_LOGE("alloc clock_component_private_t error!\n");
		return OMX_ErrorInsufficientResources;
	}
	OSCL_LOGI("========here======\n");
	/* call base init component */
	ret = base_component_init(cmp_handle);
	if (ret != OMX_ErrorNone) {
		OSCL_LOGE("base_component_init error!\n");
		goto error1;
	}
	cmp_handle->name = "OMX.LB.SOURCE.CLOCK";
	cmp_handle->component_private = private;
	cmp_handle->buf_manager = clock_buffer_manager;
	cmp_handle->buf_handle = clock_buffer_handle;
	cmp_handle->do_state_set = clock_set_state;
	cmp_handle->base_comp.ComponentDeInit = clock_component_deinit;
	cmp_handle->base_comp.SetParameter = clock_set_parameter;
	cmp_handle->base_comp.SetConfig = clock_set_config;
	cmp_handle->base_comp.GetConfig = clock_get_config;
	cmp_handle->base_comp.SetParameter = clock_set_parameter;
	cmp_handle->base_comp.GetParameter = clock_get_parameter;
	cmp_handle->num_ports = MAX_CLOCK_PORTS + DMX_SUBTITLE_STREAM_NUM;


	/* init all clock port */
	for (i = 0; i < cmp_handle->num_ports; i++) {
		ret = clock_port_init(cmp_handle, &cmp_handle->port[i], i, OMX_DirOutput);
		if (ret != OMX_ErrorNone) {
			OSCL_LOGE("clock_port %d _init error!\n", i);
			goto error2;
		}
	}

	/* init clock private data */
	private->clock_state.nSize = sizeof(OMX_TIME_CONFIG_CLOCKSTATETYPE);
	private->clock_state.eState = OMX_TIME_ClockStateStopped;
	private->clock_state.nStartTime = 0;
	private->clock_state.nOffset = 0;
	private->clock_state.nWaitMask = 0xFF;

	private->min_starttime.nSize = sizeof(OMX_TIME_CONFIG_TIMESTAMPTYPE);
	private->min_starttime.nTimestamp = 0;
	private->min_starttime.nPortIndex = 0;

	private->config_scale.nSize = sizeof(OMX_TIME_CONFIG_SCALETYPE);
	private->config_scale.xScale = 1 << 16; /* normal play mode */

	private->ref_clock.nSize = sizeof(OMX_TIME_CONFIG_ACTIVEREFCLOCKTYPE);
	private->ref_clock.eClock = OMX_TIME_RefClockNone;

	private->req_ref_clock.nSize = sizeof(OMX_TIME_CONFIG_ACTIVEREFCLOCKTYPE);
	private->req_ref_clock.eClock = OMX_TIME_RefClockNone;

	sem_init(&private->sem_starttime, 0, 0);
	sem_init(&private->sem_clockevent, 0, 0);
	sem_init(&private->sem_clkevent_complete, 0, 0);

	ref_clock_init(cmp_handle);

	return ret;

error2:
	for (j = 0; j < i; j++)
		clock_port_deinit(&cmp_handle->port[j]);
error1:
	oscl_free(private);
	return ret;
}

#ifdef OMX_DYNAMIC_LOADING
void *omx_component_init = clock_component_init;
#endif

