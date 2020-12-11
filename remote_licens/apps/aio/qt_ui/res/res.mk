PRODUCT_COPY_FILES += \
	apps/aio/qt_ui/res/db/user.db:rootfs/etc/user.db \
	apps/aio/qt_ui/res/sound/pass.wav:rootfs/usr/share/aio/res/sound/pass.wav \
	apps/aio/qt_ui/res/sound/capture_ok.wav:rootfs/usr/share/aio/res/sound/capture_ok.wav \
	apps/aio/qt_ui/res/sound/factorytest.wav:rootfs/usr/share/aio/res/sound/factorytest.wav \
	apps/aio/qt_ui/res/sound/wifi_success.wav:rootfs/usr/share/aio/res/sound/wifi_success.wav \
	apps/aio/qt_ui/res/sound/wifi_failed.wav:rootfs/usr/share/aio/res/sound/wifi_failed.wav \
	apps/aio/qt_ui/res/sound/bind.wav:rootfs/usr/share/aio/res/sound/bind.wav \
	apps/aio/qt_ui/res/sound/ble_connect.wav:rootfs/usr/share/aio/res/sound/ble_connect.wav \
	apps/aio/qt_ui/res/sound/net_link.wav:rootfs/usr/share/aio/res/sound/net_link.wav \
	apps/aio/qt_ui/res/sound/net_dislink.wav:rootfs/usr/share/aio/res/sound/net_dislink.wav \
	apps/aio/qt_ui/res/sound/scan_and_config.wav:rootfs/usr/share/aio/res/sound/scan_and_config.wav \
	apps/aio/qt_ui/res/sound/unbind.wav:rootfs/usr/share/aio/res/sound/unbind.wav \
	apps/aio/qt_ui/res/sound/alarm.wav:rootfs/usr/share/aio/res/sound/alarm.wav \
	apps/aio/qt_ui/res/elang/elang.el:rootfs/usr/share/aio/res/elang/elang.el \
	apps/aio/qt_ui/res/icons/logo.png:rootfs/etc/logo.png \

PRODUCT_COPY_DIRS += \
	apps/aio/qt_ui/res/dict:rootfs/usr/share/aio/res/dict

# include http server makefile
-include apps/aio/wechat/websocket/http/http.mk
