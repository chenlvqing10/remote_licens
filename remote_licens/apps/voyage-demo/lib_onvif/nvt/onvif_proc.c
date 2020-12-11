#include "stdsoap2.h"
#include "soapH.h"
#include "soapStub.h"

#include "onvif_param.h"
#include "onvif.h"
#include "onvif_server.h"
#include "onvif_proc.h"
#include "onvif_subscribe.h"
#include "openssl/sha.h"

#include "wsseapi.h"
#include "wsaapi.h"

#ifdef USE_DIGEST_AUTH
#include "httpda.h"
#endif

HI_ONVIF_INFO_S gOnvifInfo;

#define SOAP_UNAUTH_ERROR 401

#ifdef USE_DIGEST_AUTH
static char authrealm[] = "gufkuiopipiopupupup";
#endif

int onvif_check_security(struct soap *soap)
{
	struct _wsse__Security *wsse__Security;
	struct _wsse__UsernameToken *UsernameToken;
	int i, orgin_admin_pass_changed = 1;
	user_info_t user_info;

	if (orgin_admin_pass_changed == 0) {
		onvif_clear_wsse_head(soap);
		return SOAP_OK;
	}

#ifndef USE_WSSE_AUTH
	onvif_clear_wsse_head(soap);
#endif

#ifdef USE_WSSE_AUTH
	const char *username = soap_wsse_get_Username(soap);
	const char *password;

	printf("soap_wsse_get_Username() username %s\n", username);

	if (username == NULL) {
#ifdef USE_DIGEST_AUTH
		int ret;
		ret = onvif_check_digest_security(soap);
		return ret;
#else
		return SOAP_UNAUTH_ERROR;
#endif
	}

	if (!username) {
		/**
		 * remove old security headers before returning!
		 */
		soap_wsse_delete_Security(soap);
		/**
		 * no username: return FailedAuthentication
		 *(from soap_wsse_get_Username)
		 */
		return soap->error; 
		/*
		return SOAP_UNAUTH_ERROR;
		*/
	}

	/**
	 * lookup password of the username provided
	 */
	password = NULL;
	for (i = 0; i < MAX_USER_NUM; i++) {
		onvif_get_user_info(i, &user_info, sizeof(user_info_t));
		if ((strlen(user_info.username) > 0)
			&& (!strcmp(user_info.username, username))) {
			password = user_info.password;
			break;
		}
	}
	printf("password %s!\n", password);
	if (!password) {
		soap_wsse_delete_Security(soap);
		/**
		 * no username: return FailedAuthentication
		 * (from soap_wsse_verify_Password)
		 */
		return SOAP_UNAUTH_ERROR;
	}

	if (soap_wsse_verify_Password(soap, password)) {
		/**
		 * remove old security headers before returning!
		 */
		soap_wsse_delete_Security(soap);
		/**
		 * no username: return FailedAuthentication
		 * (from soap_wsse_verify_Password)
		 */
		return soap->error;
		/*
		return SOAP_UNAUTH_ERROR;
		*/
	}

	soap_wsse_delete_Security(soap);
#endif
	return SOAP_OK;

	if (soap->header == NULL)
		return SOAP_UNAUTH_ERROR;

	wsse__Security = soap->header->wsse__Security;

	if (wsse__Security == NULL)
		return SOAP_UNAUTH_ERROR;

	UsernameToken = wsse__Security->UsernameToken;

	if (UsernameToken == NULL)
		return SOAP_UNAUTH_ERROR;

	if (UsernameToken->Username == NULL || UsernameToken->Password == NULL)
		return SOAP_UNAUTH_ERROR;

	if (UsernameToken->Password->__item == NULL)
		return SOAP_UNAUTH_ERROR;

	if (UsernameToken->Nonce == NULL)
		return SOAP_UNAUTH_ERROR;

	if (UsernameToken->wsu__Created == NULL)
		return SOAP_UNAUTH_ERROR;

	return SOAP_OK;
}

int onvif_clear_wsse_head(struct soap *soap)
{
	const char* username = soap_wsse_get_Username(soap);

	if (username == NULL)
		return SOAP_OK;

	soap_wsse_delete_Security(soap);
	return SOAP_OK;
}

#ifdef USE_DIGEST_AUTH
int onvif_check_digest_security(struct soap* soap)
{
	char szUsrName[] = "admin";
	char szPsw[] = "admin";
	printf("user1 %s %s\n", szUsrName, szPsw);
	printf("user2 %s %s %s\n", soap->userid, soap->passwd, soap->user);
	printf("authrealm %s\n", soap->authrealm);

	/**
	 * Basic authentication: we may want to reject this
	 * since the password was sent in the clear
	 */
	if (soap->userid && soap->passwd) {
		if (!strcmp(soap->userid, szUsrName)
				&& !strcmp(soap->passwd, szPsw))
			return SOAP_OK;
	} else if (soap->authrealm && soap->userid) {
		/* simulate database lookup on userid to find passwd */
		if (!strcmp(soap->authrealm, authrealm)
				&& !strcmp(soap->userid, szUsrName)) {
			char* passwd = szPsw;
			if (!http_da_verify_post(soap, passwd))
				return SOAP_OK;
		}
	}

	soap->authrealm = authrealm;
	/**
	 *Not authorized, challenge digest authentication
	 * with httpda plugin
	 */
	return 401;
}
#endif

int onvif_check_digest_security2(struct soap *soap)
{
#ifdef USE_DIGEST_AUTH
	printf("user2 %s %s\n", soap->userid, soap->passwd);
	printf("authrealm %s\n", soap->authrealm);

	/**
	 * Basic authentication: we may want to reject this
	 * since the password was sent in the clear
	 */
	if (soap->userid && soap->passwd) {
		if (!strcmp(soap->userid, "user")
				&& !strcmp(soap->passwd, "user"))
			return SOAP_OK;
	} else if (soap->authrealm && soap->userid) {
		/* simulate database lookup on userid to find passwd */
		if (!strcmp(soap->authrealm, authrealm)
			&& !strcmp(soap->userid, "user")) {
			char* passwd = "user";
			if (!http_da_verify_post(soap, passwd))
				return SOAP_OK;
		}
	}

	soap->authrealm = authrealm;
	/**
	 * Not authorized, challenge digest authentication
	 * with httpda plugin
	 */
	return 401;
#endif
	return SOAP_OK;
}

static int onvif_add_scope(HI_ONVIF_SCOPE_S *scopes, char* item)
{
	int i;

	for (i = 0; i < ONVIF_SCOPES_NUM; i++) {
		if (scopes[i].scope[0] == 0) {
			scopes[i].isFixed = 1;
			strcpy(scopes[i].scope, item);
			return 0;
		}
	}

	return -1;
}

int onvif_get_scopes(char *str, int buflen)
{
	int i, len = 0;

	for (i = 0; i < ONVIF_SCOPES_NUM; i++) {
		if (gOnvifInfo.type_scope[i].scope[0])
			len += strlen(gOnvifInfo.type_scope[i].scope);

		if (gOnvifInfo.name_scope[i].scope[0])
			len += strlen(gOnvifInfo.type_scope[i].scope);

		if (gOnvifInfo.location_scope[i].scope[0])
			len += strlen(gOnvifInfo.type_scope[i].scope);

		if (gOnvifInfo.hardware_scope[i].scope[0])
			len += strlen(gOnvifInfo.type_scope[i].scope);
	}

	if ((len + i) > buflen)
		return -1;

	for (i = 0; i < ONVIF_SCOPES_NUM; i++) {
		if (gOnvifInfo.type_scope[i].scope[0]) {
			strcat(str, gOnvifInfo.type_scope[i].scope);
			strcat(str, " ");
		}
	}

	for (i = 0; i < ONVIF_SCOPES_NUM; i++) {
		if (gOnvifInfo.location_scope[i].scope[0]) {
			strcat(str, gOnvifInfo.location_scope[i].scope);
			strcat(str, " ");
		}
	}

	for (i = 0; i < ONVIF_SCOPES_NUM; i++) {
		if (gOnvifInfo.hardware_scope[i].scope[0]) {
			strcat(str, gOnvifInfo.hardware_scope[i].scope);
			strcat(str, " ");
		}
	}

	for (i = 0; i < ONVIF_SCOPES_NUM; i++) {
		if (gOnvifInfo.name_scope[i].scope[0]) {
			strcat(str, gOnvifInfo.name_scope[i].scope);
			strcat(str, " ");
		}
	}

	return 0;
}

static int onvif_scope_init()
{
	onvif_add_scope(gOnvifInfo.type_scope, ONVIF_SCOPE_0);
	onvif_add_scope(gOnvifInfo.type_scope, ONVIF_SCOPE_4);
	onvif_add_scope(gOnvifInfo.type_scope, ONVIF_SCOPE_5);
	onvif_add_scope(gOnvifInfo.type_scope, ONVIF_SCOPE_6);
	onvif_add_scope(gOnvifInfo.type_scope, ONVIF_SCOPE_7);
	onvif_add_scope(gOnvifInfo.type_scope, ONVIF_SCOPE_8);
	onvif_add_scope(gOnvifInfo.location_scope, ONVIF_SCOPE_1);
	onvif_add_scope(gOnvifInfo.hardware_scope, ONVIF_SCOPE_2);
	onvif_add_scope(gOnvifInfo.name_scope, ONVIF_SCOPE_3);
	return 0;
}

static int onvif_profile_init()
{
	// main profile initialize.
	gOnvifInfo.OnvifProfile[0].Name = ONVIF_MEDIA_MAIN_PROFILE_NAME;
	gOnvifInfo.OnvifProfile[0].token = ONVIF_MEDIA_MAIN_PROFILE_TOKEN;
	gOnvifInfo.OnvifProfile[0].fixed = 1;
	gOnvifInfo.OnvifProfile[0].videoSourceCfgIdx = 0;
	gOnvifInfo.OnvifProfile[0].videoEncoderCfgIdx = 0;
	gOnvifInfo.OnvifProfile[0].audioSourceCfgIdx = 0;
	gOnvifInfo.OnvifProfile[0].audioEncoderCfgIdx = 0;
	gOnvifInfo.OnvifProfile[0].ptzCfgIdx = 0;
	// sub profile initialize.
	gOnvifInfo.OnvifProfile[1].Name = ONVIF_MEDIA_SUB_PROFILE_NAME;
	gOnvifInfo.OnvifProfile[1].token = ONVIF_MEDIA_SUB_PROFILE_TOKEN;
	gOnvifInfo.OnvifProfile[1].fixed = 1;
	gOnvifInfo.OnvifProfile[1].videoSourceCfgIdx = 0;
	gOnvifInfo.OnvifProfile[1].videoEncoderCfgIdx = 1;
	gOnvifInfo.OnvifProfile[1].audioSourceCfgIdx = 0;
	gOnvifInfo.OnvifProfile[1].audioEncoderCfgIdx = 0;
	gOnvifInfo.OnvifProfile[1].ptzCfgIdx = 0;
	return 0;
}

static int onvif_video_source_init()
{
	gOnvifInfo.OnvifVideoSource[0].token = ONVIF_MEDIA_VIDEO_SOURCE_TOKEN;
	gOnvifInfo.OnvifVideoSource[0].Framerate = 30;
	gOnvifInfo.OnvifVideoSource[0].Resolution.Width = 1920;
	gOnvifInfo.OnvifVideoSource[0].Resolution.Height = 1080;

	return 0;
}

static int onvif_audio_source_init()
{
	gOnvifInfo.OnvifAudioSource[0].token = ONVIF_MEDIA_AUDIO_SOURCE_TOKEN;
	gOnvifInfo.OnvifAudioSource[0].Channels = 1;

	return 0;
}

static int onvif_video_source_cfg_init()
{
	gOnvifInfo.OnvifVideoSourceCfg[0].Name =
		ONVIF_MEDIA_VIDEO_SOURCE_TOKEN;
	gOnvifInfo.OnvifVideoSourceCfg[0].token =
		ONVIF_MEDIA_VIDEO_MAIN_CFG_TOKEN;
	gOnvifInfo.OnvifVideoSourceCfg[0].UseCount = 1;
	gOnvifInfo.OnvifVideoSourceCfg[0].SourceToken = ONVIF_MEDIA_VIDEO_SOURCE_TOKEN;
	gOnvifInfo.OnvifVideoSourceCfg[0].Bounds.x = 0;
	gOnvifInfo.OnvifVideoSourceCfg[0].Bounds.y = 0;
	gOnvifInfo.OnvifVideoSourceCfg[0].Bounds.width = 1920;
	gOnvifInfo.OnvifVideoSourceCfg[0].Bounds.height = 1080;
	
	gOnvifInfo.OnvifVideoSourceCfg[1].Name =
		ONVIF_MEDIA_VIDEO_SOURCE_TOKEN;
	gOnvifInfo.OnvifVideoSourceCfg[1].token =
		ONVIF_MEDIA_VIDEO_MAIN_CFG_TOKEN;
	gOnvifInfo.OnvifVideoSourceCfg[1].UseCount = 1;
	gOnvifInfo.OnvifVideoSourceCfg[1].SourceToken = ONVIF_MEDIA_VIDEO_SOURCE_TOKEN;
	gOnvifInfo.OnvifVideoSourceCfg[1].Bounds.x = 0;
	gOnvifInfo.OnvifVideoSourceCfg[1].Bounds.y = 0;
	gOnvifInfo.OnvifVideoSourceCfg[1].Bounds.width = 1920;
	gOnvifInfo.OnvifVideoSourceCfg[1].Bounds.height = 1080;

	return 0;
}

static int onvif_video_encoder_cfg_init()
{
	gOnvifInfo.OnvifVideoEncoderCfg[0].Name = ONVIF_MEDIA_VIDEO_MAIN_ENCODER_NAME;
	gOnvifInfo.OnvifVideoEncoderCfg[0].token = ONVIF_MEDIA_VIDEO_MAIN_ENCODER_TOKEN;
	gOnvifInfo.OnvifVideoEncoderCfg[0].UseCount = 1;
	gOnvifInfo.OnvifVideoEncoderCfg[0].Encoding = tt__VideoEncoding__H264;
	gOnvifInfo.OnvifVideoEncoderCfg[0].Resolution.Width = 1920;
	gOnvifInfo.OnvifVideoEncoderCfg[0].Resolution.Height = 1080;
	gOnvifInfo.OnvifVideoEncoderCfg[0].Quality = 1;
	gOnvifInfo.OnvifVideoEncoderCfg[0].SessionTimeout = "PT5S";
	gOnvifInfo.OnvifVideoEncoderCfg[0].Gop = 30;
	gOnvifInfo.OnvifVideoEncoderCfg[0].FrameRate = 30;
	gOnvifInfo.OnvifVideoEncoderCfg[0].Bitrate = 2000;
	gOnvifInfo.MediaConfig.h264Options[0].Height = 1080;
	gOnvifInfo.MediaConfig.h264Options[0].Width = 1920;
	gOnvifInfo.MediaConfig.VideoResolutionconfig =
		gOnvifInfo.MediaConfig.h264Options[0];
	gOnvifInfo.MediaConfig.VideoEncoderRateControl.BitrateLimit = 2000;
	gOnvifInfo.MediaConfig.VideoEncoderRateControl.FrameRateLimit = 30;
	gOnvifInfo.MediaConfig.VideoEncoderRateControl.EncodingInterval = 1;
	
	gOnvifInfo.OnvifVideoEncoderCfg[1].Name = ONVIF_MEDIA_VIDEO_SUB_ENCODER_NAME;
	gOnvifInfo.OnvifVideoEncoderCfg[1].token = ONVIF_MEDIA_VIDEO_SUB_ENCODER_TOKEN;
	gOnvifInfo.OnvifVideoEncoderCfg[1].UseCount = 1;
	gOnvifInfo.OnvifVideoEncoderCfg[1].Encoding = tt__VideoEncoding__H264;
	gOnvifInfo.OnvifVideoEncoderCfg[1].Resolution.Width = 1280;
	gOnvifInfo.OnvifVideoEncoderCfg[1].Resolution.Height = 720;
	gOnvifInfo.OnvifVideoEncoderCfg[1].Quality = 1;
	gOnvifInfo.OnvifVideoEncoderCfg[1].SessionTimeout = "PT5S";
	gOnvifInfo.OnvifVideoEncoderCfg[1].Gop = 30;
	gOnvifInfo.OnvifVideoEncoderCfg[1].FrameRate = 30;
	gOnvifInfo.OnvifVideoEncoderCfg[1].Bitrate = 1000;
	gOnvifInfo.MediaConfig.h264Options[1].Width = 1280;
	gOnvifInfo.MediaConfig.h264Options[1].Height = 720;

	return 0;
}

static int onvif_audio_source_cfg_init()
{
	gOnvifInfo.OnvifAudioSourceCfg[0].Name = ONVIF_MEDIA_AUDIO_CFG_NAME;
	gOnvifInfo.OnvifAudioSourceCfg[0].token = ONVIF_MEDIA_AUDIO_CFG_TOKEN;
	gOnvifInfo.OnvifAudioSourceCfg[0].UseCount = 2;
	gOnvifInfo.OnvifAudioSourceCfg[0].SourceToken = ONVIF_MEDIA_AUDIO_SOURCE_TOKEN;
	return 0;
}

static int onvif_audio_encoder_cfg_init()
{
	gOnvifInfo.OnvifAudioEncoderCfg[0].Name = ONVIF_MEDIA_AUDIO_ENCODER_NAME;
	gOnvifInfo.OnvifAudioEncoderCfg[0].token = ONVIF_MEDIA_AUDIO_ENCODER_TOKEN;
	gOnvifInfo.OnvifAudioEncoderCfg[0].UseCount = 1;
	gOnvifInfo.OnvifAudioEncoderCfg[0].Encoding = tt__AudioEncoding__G711;
	gOnvifInfo.OnvifAudioEncoderCfg[0].Bitrate = 16000;
	gOnvifInfo.OnvifAudioEncoderCfg[0].SampleRate = 8000;
	gOnvifInfo.OnvifAudioEncoderCfg[0].SessionTimeout = "PT5S";
	return 0;
}

static int onvif_ptz_cfg_init()
{
	int i;

	for (i = 0; i < ONVIF_PTZ_PRESET_NUM; i++)
		sprintf(gOnvifInfo.OnvifPTZCfg[0].presetCfg[i].Name,
			"%s_%d", ONVIF_PTZ_NAME, i);

	for (i = 0; i < ONVIF_PTZ_PRESET_NUM; i++)
		sprintf(gOnvifInfo.OnvifPTZCfg[0].presetCfg[i].token,
		"%s_%d", ONVIF_PTZ_TOKEN, i);

	return 0;
}

int onvif_server_init(int nHttpPort)
{
	__FUN_BEGIN("\n");
	memset(&gOnvifInfo, 0, sizeof(gOnvifInfo));
	gOnvifInfo.HttpPort = nHttpPort;
#ifdef EXTERNAL_SVR_PORT
	__D("Use define port %d for onvif external port.\n", EXTERNAL_SVR_PORT);
	gOnvifInfo.HttpPort = EXTERNAL_SVR_PORT;
#endif
	onvif_scope_init();
	onvif_profile_init();
	onvif_video_source_init();
	onvif_audio_source_init();
	onvif_video_source_cfg_init();
	onvif_video_encoder_cfg_init();
	onvif_audio_source_cfg_init();
	onvif_audio_encoder_cfg_init();
	onvif_ptz_cfg_init();
	tev_Init();
	__FUN_END("\n");
	return 0;
}

