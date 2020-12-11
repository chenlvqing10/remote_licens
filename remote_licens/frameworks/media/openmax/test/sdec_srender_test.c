#define DBG_LEVEL		DBG_LOG	/* DBG_INFO */
#define LOG_TAG			"sdec_srender_test"

#include <oscl.h>
#include <base_component.h>
#include <lb_omx_core.h>
#include "srender_component.h"
#include "sdec_component.h"
#include "omx_test.h"
#define INPUT_BUF_NUM		8
#define OUTPUT_BUF_NUM		8
#define SUBTITLE_INBUF_DEFAULT_SIZE	1024

typedef struct sdec_srender_test {
	OMX_COMPONENTTYPE *source;	/* subtitle decode component */
	OMX_COMPONENTTYPE *sink;	/* subtitle render component */
	OMX_PARAM_PORTDEFINITIONTYPE source_port_in_para;
	OMX_PARAM_PORTDEFINITIONTYPE source_port_out_para;
	OMX_PARAM_PORTDEFINITIONTYPE sink_para;
	int port_src_in;		/* subtitle decode component input port */
	int port_src_out;		/* subtitle decode component output port */
	int port_sink_in;		/* subtitle render component input port */
	FILE	*infile;
	sem_t sem_event;
	sem_t sem_eos;
	oscl_queue_t		buffer_queue;
	OMX_S32 buf_num;		/* buffer number to sensor */
	OMX_BUFFERHEADERTYPE **buffer;
	OMX_BUFFERHEADERTYPE **source_buffer;
	OMX_U32 error_value;
	int read_thread_exit;
} sdec_srender_test_t;

static sdec_srender_test_t sdec_srender_priv;

static OMX_ERRORTYPE sdec_event_handler(
	OMX_HANDLETYPE hComponent,
	OMX_PTR pAppData,
	OMX_EVENTTYPE eEvent,
	OMX_U32 Data1,
	OMX_U32 Data2,
	OMX_PTR pEventData)
{
	sdec_srender_test_t *private = (sdec_srender_test_t *)pAppData;
	char *name;
	OSCL_TRACE("hComponent:%p", hComponent);
	if (hComponent == private->sink)
		name = "sink";
	if (hComponent == private->source)
		name = "source";
	if (eEvent == OMX_EventCmdComplete) {
		if (Data1 == OMX_CommandStateSet) {
			switch ((int)Data2) {
			case OMX_StateInvalid:
				OSCL_LOGI("%s StateSet OMX_StateInvalid", name);
				break;
			case OMX_StateLoaded:
				OSCL_LOGI("%s StateSet OMX_StateLoaded", name);
				break;
			case OMX_StateIdle:
				OSCL_LOGI("%s StateSet OMX_StateIdle", name);
				break;
			case OMX_StateExecuting:
				OSCL_LOGI("%s StateSet OMX_StateExecuting", name);
				break;
			case OMX_StatePause:
				OSCL_LOGI("%s StateSet OMX_StatePause", name);
				break;
			case OMX_StateWaitForResources:
				OSCL_LOGI("%s StateSet OMX_StateWaitForResources", name);
				break;
			default:
				OSCL_LOGI("%s StateSet unkown state", name);
				break;
			}
			sem_post(&private->sem_event);
		} else if (Data1 == OMX_CommandPortEnable) {
			OSCL_LOGI("%s CmdComplete OMX_CommandPortEnable", name);
			sem_post(&private->sem_event);
		} else if (Data1 == OMX_CommandPortDisable) {
			OSCL_LOGI("%s CmdComplete OMX_CommandPortDisable", name);
			sem_post(&private->sem_event);
		}
	} else if (eEvent == OMX_EventBufferFlag) {
		if ((int)Data2 == OMX_BUFFERFLAG_EOS) {
			OSCL_LOGI("%s BufferFlag OMX_BUFFERFLAG_EOS", name);
			if (hComponent == private->sink) {
				OSCL_LOGE("end of tunnel");
				sem_post(&private->sem_eos);
			}
		} else
			OSCL_LOGI("%s OMX_EventBufferFlag %x", name, Data2);
	} else if (eEvent == OMX_EventError) {
		private->error_value = Data1;
		OSCL_LOGE("decode error event. value: %x", private->error_value);
		sem_post(&private->sem_event);
	} else {
		OSCL_LOGI("%s param: %x %x", name, Data1, Data2);
	}
	return OMX_ErrorNone;
}

static OMX_ERRORTYPE sdec_untunnel_fill_buffer_done(
	OMX_HANDLETYPE hComponent,
	OMX_PTR pAppData,
	OMX_BUFFERHEADERTYPE *pBuffer) {
	OMX_ERRORTYPE err;
	sdec_srender_test_t *priv = pAppData;
	if (pBuffer == NULL || pAppData == NULL) {
		OSCL_LOGE("err: buffer header is null");
		return OMX_ErrorBadParameter;
	}
	OSCL_LOGD("fill buffer done(%s)>> %p, %d, input:%d output:%d",
			get_component_name((get_lb_component(hComponent))),
			pBuffer->pBuffer,
			pBuffer->nFlags,
			pBuffer->nInputPortIndex,
			pBuffer->nOutputPortIndex);
	pBuffer->nInputPortIndex = SINK_INPUT_PORT_INDEX;
	err = OMX_EmptyThisBuffer(priv->sink, pBuffer);
	if (err != OMX_ErrorNone)
		OSCL_LOGE("OMX_EmptyThisBuffer err: %x", err);
	return err;
}

static OMX_ERRORTYPE sdec_untunnel_empty_buffer_done(
	OMX_HANDLETYPE hComponent,
	OMX_PTR pAppData,
	OMX_BUFFERHEADERTYPE *pBuffer)
{
	OMX_ERRORTYPE err;
	sdec_srender_test_t *priv = pAppData;
	if (pBuffer == NULL || pAppData == NULL) {
		OSCL_LOGE("err: buffer header is null");
		return OMX_ErrorBadParameter;
	}
	OSCL_LOGD("empty buffer done(%s)>> %p, %d, input: %d output: %d",
			get_component_name((get_lb_component(hComponent))),
			pBuffer->pBuffer,
			pBuffer->nFlags,
			pBuffer->nInputPortIndex,
			pBuffer->nOutputPortIndex);
	pBuffer->nOutputPortIndex = SOURCE_OUTPUT_PORT_INDEX;
	err = OMX_FillThisBuffer(priv->source, pBuffer);
	if (err != OMX_ErrorNone)
		OSCL_LOGE("OMX_FillThisBuffer err: %x", err);
	return err;

}

static OMX_CALLBACKTYPE sdec_untunnel_callbacks = {
	.EventHandler = sdec_event_handler,
	.EmptyBufferDone = sdec_untunnel_empty_buffer_done,
	.FillBufferDone = sdec_untunnel_fill_buffer_done
};

static OMX_ERRORTYPE alloc_buffer(sdec_srender_test_t *priv,
			OMX_S32 sel_comp, OMX_S32 num, OMX_S32 size)
{
	OMX_S32 i = 0;
	OMX_S32 port_index = 0;
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	OMX_BUFFERHEADERTYPE **buffer;
	OMX_COMPONENTTYPE *comp = NULL;

	buffer = oscl_zalloc(num * sizeof(OMX_BUFFERHEADERTYPE *));
	if (NULL == buffer)
		return OMX_ErrorBadParameter;
	if (0 == sel_comp) {
		comp = priv->source;
		port_index = SOURCE_INPUT_PORT_INDEX;
	} else {
		comp = priv->source;
		port_index = SOURCE_OUTPUT_PORT_INDEX;
	}
	for (i = 0; i < num; i++) {
		buffer[i] = NULL;
		ret = OMX_AllocateBuffer(comp, &buffer[i],
				port_index, priv, size);
		OSCL_LOGI("AllocateBuffer %p on port %i", buffer[i], port_index);
		if (ret != OMX_ErrorNone) {
			OSCL_LOGE("Error on AllocateBuffer %p on port %i\n",
				&buffer[i], port_index);
			break;
		}
	}
	if (0 == sel_comp) {
		priv->buffer = buffer;
		for (i = 0; i < num; i++) {
			ret = oscl_queue_queue(&priv->buffer_queue, priv->buffer[i]);
			if (ret != 0) {
				OSCL_LOGE("queue buffer %d error!\n", i);
				break;
			}
		}
	} else
		priv->source_buffer = buffer;

	return ret;
}

static void free_buffer(sdec_srender_test_t *priv,  OMX_S32 sel_comp)
{
	OMX_S32 i = 0;
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	OMX_S32 port_index = 0;
	OMX_BUFFERHEADERTYPE **buffer;

	if (0 == sel_comp) {
		buffer = priv->buffer;
		port_index = SOURCE_INPUT_PORT_INDEX;
	} else {
		buffer = priv->source_buffer;
		port_index = SOURCE_OUTPUT_PORT_INDEX;
	}

	if (buffer) {
		for (i = 0; i < priv->buf_num; i++) {
			if (buffer[i]) {
				ret = OMX_FreeBuffer(priv->source,
						port_index,
						buffer[i]);
				if (ret != OMX_ErrorNone)
					OSCL_LOGE("port %d ,freebuffer:%d failed",
						port_index, i);
			}
			buffer[i] = NULL;
		}
		oscl_free(buffer);
		buffer = NULL;
	}
}

static int untunnel_config_source_component(sdec_srender_test_t *priv)
{
	OSCL_LOGI("here");
	OMX_ERRORTYPE ret = OMX_ErrorNone;

	/* 1. config input port para*/
	memset(&priv->source_port_in_para, 0, sizeof(priv->source_port_in_para));
	priv->buf_num = INPUT_BUF_NUM;
	priv->source_port_in_para.nPortIndex = SOURCE_INPUT_PORT_INDEX;
	priv->source_port_in_para.nBufferCountActual = priv->buf_num;
	priv->source_port_in_para.bBuffersContiguous = 1;
	priv->source_port_in_para.eDomain = OMX_PortDomainSubtitle;
	priv->source_port_in_para.nBufferSize = SUBTITLE_INBUF_DEFAULT_SIZE;
	priv->source_port_in_para.nBufferAlignment = 1024;
	ret = OMX_SetParameter(priv->source, OMX_IndexParamPortDefinition,
				&priv->source_port_in_para);
	if (ret) {
		OSCL_LOGE("set priv->source_port_in_para params error!");
		return ret;
	}
	ret = alloc_buffer(priv, 0, priv->source_port_in_para.nBufferCountActual,
			priv->source_port_in_para.nBufferSize);
	/* 2. config output port para*/
	memset(&priv->source_port_out_para, 0, sizeof(priv->source_port_out_para));
	priv->source_port_out_para.nVersion.nVersion = OMX_VERSION;
	priv->source_port_out_para.nPortIndex = SOURCE_OUTPUT_PORT_INDEX;
	ret = OMX_GetParameter(priv->source, OMX_IndexParamPortDefinition,
				&priv->source_port_out_para);
	if (ret != OMX_ErrorNone) {
		OSCL_LOGE("get params error!\n");
		return ret;
	}
	priv->source_port_out_para.nBufferCountActual = INPUT_BUF_NUM;
	priv->source_port_out_para.nBufferAlignment = 1024;
	ret = OMX_SetParameter(priv->source, OMX_IndexParamPortDefinition,
				&priv->source_port_out_para);
	oscl_param_check((ret == OMX_ErrorNone), -1, "SET port definition err\n");
	/* 3. get param after source */
	priv->source_port_out_para.nVersion.nVersion = OMX_VERSION;
	priv->source_port_out_para.nPortIndex = SOURCE_OUTPUT_PORT_INDEX;
	ret = OMX_GetParameter(priv->source, OMX_IndexParamPortDefinition,
			&priv->source_port_out_para);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);

	OSCL_LOGI("priv->source_port_out_para.nBufferSize:%d",
		priv->source_port_out_para.nBufferSize);
	OSCL_LOGI("priv->source_port_out_para.nBufferCountActual:%d",
			priv->source_port_out_para.nBufferCountActual);
	ret = alloc_buffer(priv, 1, priv->source_port_out_para.nBufferCountActual,
			priv->source_port_out_para.nBufferSize);
EXIT:
	return ret;
}

static void untunnel_exit(sdec_srender_test_t *priv)
{
	if (priv->source) {
		OMX_FreeHandle(priv->source);
		priv->source = NULL;
	}
	if (priv->sink) {
		OMX_FreeHandle(priv->sink);
		priv->sink = NULL;
	}
	sem_destroy(&priv->sem_event);
	sem_destroy(&priv->sem_eos);
	OSCL_LOGD("here");
}

static int untunnel_config_sink_component(sdec_srender_test_t *priv)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	memcpy(&priv->sink_para, &priv->source_port_out_para,
			sizeof(priv->source_port_out_para));
	priv->sink_para.nVersion.nVersion = OMX_VERSION;
	priv->sink_para.nPortIndex = SINK_INPUT_PORT_INDEX;
	ret = OMX_SetParameter(priv->sink, OMX_IndexParamPortDefinition,
			&priv->sink_para);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);
EXIT:
	return ret;
}
static void *untunnel_sdec_srender_thread(void *param)
{
	sdec_srender_test_t *priv = &sdec_srender_priv;
	int ret = OMX_ErrorNone;
	OMX_BUFFERHEADERTYPE *buffer;
	OMX_BOOL eof = OMX_FALSE;
	int rlen;
	OSCL_LOGE("untunnel_sdec_srender_thread access");
	while (1) {
		if (priv->read_thread_exit || eof) {
			OSCL_LOGI("exit read loop!\n");
			break;
		}
		/* dequeue one buffer */
		buffer = oscl_queue_dequeue(&priv->buffer_queue);
		if (buffer == NULL)
			continue;
		/* read subtitle file */
		rlen = fread(buffer->pBuffer, 1, buffer->nAllocLen, priv->infile);
		OSCL_LOGE("read len %d buffer %s..\n", rlen, buffer->pBuffer);
		if (rlen <= 0) {
			OSCL_LOGE("read file error rlen %d!\n", rlen);
			eof = OMX_TRUE;
			buffer->nFilledLen = 0;
			buffer->nOffset = 0;
			buffer->nFlags = OMX_BUFFERFLAG_EOS;
		} else {
			buffer->nFilledLen = rlen;
			buffer->nOffset = 0;
			buffer->nFlags = 0;
		}
		buffer->nInputPortIndex = SOURCE_INPUT_PORT_INDEX;
		OSCL_LOGE("source empty this buffer len:%d", rlen);
		ret = OMX_EmptyThisBuffer(priv->source, buffer);
		if (ret != OMX_ErrorNone) {
			OSCL_LOGE("source empty this buffer error!\n");
			break;
		}
	}
	OSCL_LOGI("end of untunnel_sdec_srender_thread!\n");
	return NULL;
}
#if 1
int openmax_test_sdec_srender_untunnel(char *filename)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	sdec_srender_test_t *priv = &sdec_srender_priv;
	pthread_t thread_id;
	OMX_PARAM_PORTDEFINITIONTYPE port_def;
	OSCL_LOGI("here");
	int i;
	memset(priv, 0, sizeof(sdec_srender_test_t));
	oscl_queue_init(&priv->buffer_queue);

	priv->infile = fopen(filename, "rb");
	if (!priv->infile) {
		OSCL_LOGE("open file error!\n");
		return -1;
	}
	/* 1. get component handle */
	OMX_GetHandle((void **)&priv->source,
		"OMX.LB.SINK.SDEC", priv, &sdec_untunnel_callbacks);
	OMX_GetHandle((void **)&priv->sink,
		"OMX.LB.SINK.SRENDER", priv, &sdec_untunnel_callbacks);
	OSCL_LOGD("here");
	if (priv->sink == NULL || priv->source == NULL) {
		OSCL_LOGE("get handle failed! %p, %p\n", priv->source, priv->sink);
		fclose(priv->infile);
		return -1;
	}
	OSCL_LOGI("here");
	sem_init(&priv->sem_event, 0, 0);
	sem_init(&priv->sem_eos, 0, 0);

	/* 2. config sink component */
	ret = untunnel_config_source_component(priv);
	oscl_param_check_exit((ret == OMX_ErrorNone), ret, NULL);

	ret = untunnel_config_sink_component(priv);
	oscl_param_check_exit((ret == OMX_ErrorNone), ret, NULL);

	/* 3. set component stat to idle */
	ret = OMX_SendCommand(priv->source, OMX_CommandStateSet, OMX_StateIdle, NULL);
	oscl_param_check_exit((ret == OMX_ErrorNone), ret, NULL);
	sem_wait(&priv->sem_event);

	ret = OMX_SendCommand(priv->sink, OMX_CommandStateSet, OMX_StateIdle, NULL);
	oscl_param_check_exit((ret == OMX_ErrorNone), ret, NULL);
	sem_wait(&priv->sem_event);

	/* 4. send buffer to decode component queue */
	port_def.nVersion.nVersion = OMX_VERSION;
	port_def.nPortIndex = SOURCE_OUTPUT_PORT_INDEX;
	ret = OMX_GetParameter(priv->source, OMX_IndexParamPortDefinition, &port_def);
	if (ret != OMX_ErrorNone) {
		OSCL_LOGE("Error when getting OMX_PORT_PARAM_TYPE,%x\n", ret);
		goto EXIT;
	}

	OSCL_LOGD("port_def.nBufferCountActual:%d", port_def.nBufferCountActual);
	for (i = 0; i < port_def.nBufferCountActual; i++) {
		ret = OMX_FillThisBuffer(priv->source, priv->source_buffer[i]);
		oscl_param_check_exit((ret == OMX_ErrorNone), ret, NULL);

		OSCL_LOGI("OMX_FillThisBuffer %p on port %i",
				priv->source_buffer[i],
				priv->source_buffer[i]->nOutputPortIndex);
	}

	/* 4. set component stat to executing */
	ret = OMX_SendCommand(priv->source,
		OMX_CommandStateSet, OMX_StateExecuting, NULL);
	oscl_param_check_exit((ret == OMX_ErrorNone), ret, NULL);
	sem_wait(&priv->sem_event);
	ret = OMX_SendCommand(priv->sink,
		OMX_CommandStateSet, OMX_StateExecuting, NULL);
	oscl_param_check_exit((ret == OMX_ErrorNone), ret, NULL);
	sem_wait(&priv->sem_event);
#if 1
	/* 5. start untunnel_sdec_srender_thread */
	pthread_create(&thread_id, NULL, untunnel_sdec_srender_thread, (void *)priv);
	OSCL_LOGI("wait for sem eos!\n");
	sem_wait(&priv->sem_eos);
	OSCL_LOGI("we have eos signal!\n");

	/* 6. set component stat to idle */
	ret = OMX_SendCommand(priv->source, OMX_CommandStateSet, OMX_StateIdle, NULL);
	oscl_param_check_exit((ret == OMX_ErrorNone), ret, NULL);
	sem_wait(&priv->sem_event);
	ret = OMX_SendCommand(priv->sink, OMX_CommandStateSet, OMX_StateIdle, NULL);
	oscl_param_check_exit((ret == OMX_ErrorNone), ret, NULL);
	sem_wait(&priv->sem_event);

	/* 7. end the untunnel_sdec_srender_thread thread */
	priv->read_thread_exit = 1;
	pthread_join(thread_id, NULL);

	/* 8. free buffer */
	free_buffer(priv, 0);
	free_buffer(priv, 1);

	/* 9. set component stat to loaded */
	ret = OMX_SendCommand(priv->source, OMX_CommandStateSet, OMX_StateLoaded, NULL);
	oscl_param_check_exit((ret == OMX_ErrorNone), ret, NULL);
	sem_wait(&priv->sem_event);
	ret = OMX_SendCommand(priv->sink, OMX_CommandStateSet, OMX_StateLoaded, NULL);
	oscl_param_check_exit((ret == OMX_ErrorNone), ret, NULL);
	sem_wait(&priv->sem_event);
	OSCL_LOGE("loaded ");
#endif
EXIT:
	fclose(priv->infile);
	untunnel_exit(priv);
	return ret;


}
#endif

static omx_component_tbl_t omx_sdec_component = {
	"OMX.LB.SINK.SDEC",
	1,
	sdec_component_init, NULL, NULL,
	{
		"sink.sdec",
		NULL
	}
};

static omx_component_tbl_t omx_srender_component = {
	"OMX.LB.SINK.SRENDER",
	1,
	srender_component_init, NULL, NULL,
	{
		"sink.srender",
		NULL
	}
};

int main(int argc, char **argv)
{
	/* init omx and regster components */
	OMX_Init();
	omx_component_register(&omx_sdec_component);
	omx_component_register(&omx_srender_component);
	openmax_test_sdec_srender_untunnel(argv[1]);
}

#ifdef __EOS__
int srender_component_test(int argc, char **argv)
{
	/* init omx and regster components */
	OMX_Init();
	omx_component_register(&omx_sdec_component);
	omx_component_register(&omx_srender_component);
	openmax_test_sdec_srender_untunnel(argv[1]);
	return 0;
}

MSH_CMD_EXPORT(srender_component_test, "srender_component_test");

#endif

