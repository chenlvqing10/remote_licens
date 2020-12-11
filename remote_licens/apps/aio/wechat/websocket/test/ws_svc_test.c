#if 1
#include "ws_svc.h"
#include "ws_app_main.h"
#include "ws_app_util.h"
static sig_atomic_t s_signal_received = 0;

static void signal_handler(int sig_num)
{
	signal(sig_num, signal_handler);
	s_signal_received = sig_num;
}

int recv_hander(unsigned char *data, size_t len, unsigned char flags)
{
	printf("recv_hander\n");
	switch (flags) {
	case WEBSOCKET_OP_TEXT:
		ws_app_handle_text(data, len, flags);
		break;
	case WEBSOCKET_OP_BINARY:
		break;
	default:
		break;
	}
	return 0;
}

int success_hander(int status)
{
	printf("success_hander\n");
	ws_app_sync_server_timestamp();
	ws_app_device_register("E30000000001", "708d0b6f4d4bd97e5d4eb1cb47d260ac");
	//ws_app_device_active("lk9BZOIBUxzD4jMjSiath3seI7YPxxEFoU-SO1SglG3mHwGDv_viAlt1P59W6gsV");
	ws_app_device_subscribe_corp("dJoEGgt9NCzygTzwGwkzVvmlLy6Hc4P2DICISBTZ34o");
	return 0;
}

int reset_hander(int status)
{
	printf("reset_hander\n");
	return 0;
}
int error_hander(int status)
{
	printf("error_hander\n");
	return 0;
}

int main(int argc, char *argv[])
{
	void *ws_svc = NULL;
	char *server_url = argc > 1 ? argv[1] : "wss://openhw.work.weixin.qq.com/";
	char *protocol = argc > 2 ? argv[2] : "protocol";

	printf("server_url = %s\n", server_url);
	printf("protocol = %s\n", protocol);

	signal(SIGTERM, signal_handler);
	signal(SIGINT, signal_handler);

	ws_svc = ws_svc_init(server_url, strlen(server_url),
			protocol, strlen(protocol));
	if (!ws_svc)
		return -1;

	ws_svc_set_net_linked();

	//char *ping = "{\"cmd\":\"ping\",\"headers\":{\"req_id\":\"ping\"}}";
	//ws_register_timer_text(2, (unsigned char *)ping, strlen(ping));
	ws_app_register_ping(40);
	ws_app_register_get_timestamp(40);
	ws_register_recv_cb(recv_hander);
	ws_svc_set_net_linked();

	ws_register_nc_success_cb(success_hander);
	ws_register_nc_reset_cb(reset_hander);
	ws_register_nc_error_cb(error_hander);

	//ws_app_device_get_secret_no("E30000000001", "dbd7f84e64ec64708df6df39aadfb039");
	//ws_app_device_dynamic_login_qrcode("E30000000001", "708d0b6f4d4bd97e5d4eb1cb47d260ac");

	while (s_signal_received == 0) {
		char text[4096] = {0};

		fgets(text, 4096, stdin);
		printf("\ntext:\n%s", text);
		if (strlen(text) > 1)
			ws_send_text((unsigned char *)text, strlen(text));
	}
	ws_svc_deinit();
	return 0;
}

#endif
