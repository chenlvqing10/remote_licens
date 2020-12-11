LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

QT_RESOURCE := $(shell $(LOCAL_PATH)/qt_resource.sh)

LOCAL_MODULE := aio

LOCAL_MODULE_TAGS := optional

QT_FILES_PATH  := \
	$(LOCAL_PATH)/qt_ui/qt_generate  \

QT_FILES_SUFFIX := %.cpp

QT_All_Files := $(foreach src_path,$(QT_FILES_PATH), $(shell find $(src_path) -type f \( ! -iname "*_eos_*" \)))
QT_All_Files := $(QT_All_Files:$(MY_CPP_PATH)/./%=$(MY_CPP_PATH)%)
QT_SRC_LIST  := $(filter $(QT_FILES_SUFFIX),$(QT_All_Files))
QT_SRC_LIST  := $(QT_SRC_LIST:$(LOCAL_PATH)/%=%)
#$(info source:$(QT_SRC_LIST))

LOCAL_SRC_FILES := \
	base/file/file_ops.c \
	base/file/file_http_download.c \
	base/queue/ee_queue.c \
	base/queue/queue.c \
	base/time/time_conv.c \
	base/time/time_ntp.c \
	base/string/char_conv.c \
	base/string/char_ops.c \
	base/base64/base64.c \
	base/json/cJSON.c \
	base/storage/hotplug.c \
	base/storage/storage.c \
	base/md5/dd_md5.c \
	base/md5/dd_md5-internal.c \
	base/md5/md5.c \
	base/excel/excel_file_read.c \
	base/excel/excel_file_write.c \
	base/excel/excel_person_public.c \
	base/clang/clang.c \
	face/face.c \
	api/hal/display.c \
	api/hal/gpio.c \
	api/hal/led.c \
	api/hal/key.c \
	api/hal/isp.c \
	api/hal/sensor.c \
	api/hal/relay.c \
	api/hal/wiegand_read.c \
	api/hal/wiegand_write.c \
	api/hal/watchdog.c \
	api/hal/rs.c \
	api/system/system_private.c \
	api/system/system_factory.c \
	api/system/system_update.c \
	api/system/system_volume.c \
	api/system/system_info.c \
	api/system/system_chip_id.c \
	api/factory/factory_test.c \
	api/management/person_import.c \
	api/management/person_output.c \
	api/management/person.c \
	api/management/person_reg_check.c \
	api/live_video/src/frame_queue.c \
	api/live_video/src/live_video.c \
	api/live_video/src/live_video_ctrl.c \
	api/live_video/src/live_server.cpp \
	api/live_video/src/mem_pool.c \
	api/live_video/src/osa.c \
	api/live_video/src/H264VideoServerMediaSubsession.cpp \
	api/live_video/src/H264VideoSource.cpp \
	services/src/reset_svc.c \
	services/src/update_svc.c \
	services/src/light_svc.c \
	services/src/screen_svc.c \
	services/src/sound_svc.c \
	services/src/alarm_svc.c \
	services/src/open_door_svc.c \
	services/src/checkin_export_svc.c \
	services/src/test_log_svc.c \
	services/src/wiegand_svc.c \
	services/src/reboot_svc.c \
	services/src/ota_svc.c \
	wechat/websocket/core/ws_core_log.c \
	wechat/websocket/core/ws_core_main.c \
	wechat/websocket/mongoose/mongoose.c  \
	wechat/ws_control.c \
	wechat/ws_svc_main.c \
	wechat/ws_bt_wifi.c \
	wechat/websocket/http/http_main.c \
	wechat/websocket/http/webapi/web_biz_api.c \
	wechat/websocket/http/webapi/ws_http.c \
	wechat/websocket/http/webapi/http_client.c \
	qt_ui/src/signal/signal_for_ui.cpp \
	qt_ui/src/sql/db_all.cpp \
	qt_ui/src/control/ui_control.c \
	qt_ui/src/main/main.cpp \
	qt_ui/src/main/signal_table.cpp \
	qt_ui/src/main/slot_table_test.cpp \
	qt_ui/src/sql/operator/sqlapi.cpp \
	qt_ui/src/sql/base/sqlbase.cpp \
	qt_ui/src/sql/tables/biitem.cpp \
	qt_ui/src/sql/tables/checkin.cpp \
	qt_ui/src/sql/tables/exportcheckin.cpp \
	qt_ui/src/sql/tables/person.cpp \
	qt_ui/src/sql/tables/rule.cpp \
	qt_ui/src/ui/QtMainWindow.cpp \
	qt_ui/src/ui/QtDialog.cpp \
	qt_ui/src/ui/setting/user/ui_add_user.cpp \
	qt_ui/src/ui/setting/user/ui_add_user_delegate.cpp \
	qt_ui/src/ui/setting/user/ui_user_management.cpp \
	qt_ui/src/ui/setting/user/ui_user_management_delegate.cpp \
	qt_ui/src/ui/setting/user/ui_user_view_list.cpp \
	qt_ui/src/ui/setting/algorithm/ui_algorithm_setting.cpp \
	qt_ui/src/ui/setting/algorithm/ui_algorithm_setting_delegate.cpp \
	qt_ui/src/ui/setting/attendance/ui_attendance_setting.cpp \
	qt_ui/src/ui/setting/attendance/ui_attendance_list.cpp \
	qt_ui/src/ui/setting/attendance/ui_attendance_data.cpp \
	qt_ui/src/ui/setting/attendance/ui_attendance_search.cpp \
	qt_ui/src/ui/setting/attendance/ui_setting_item_delegate.cpp \
	qt_ui/src/ui/setting/debug/ui_batch_import.cpp \
	qt_ui/src/ui/setting/debug/ui_batch_register.cpp \
	qt_ui/src/ui/setting/debug/ui_batch_recognition.cpp \
	qt_ui/src/ui/setting/debug/ui_debug_switch.cpp \
	qt_ui/src/ui/setting/debug/ui_debug_switch_delegate.cpp \
	qt_ui/src/ui/setting/network/ui_eth_setting.cpp \
	qt_ui/src/ui/setting/network/ui_eth_setting_delegate.cpp \
	qt_ui/src/ui/setting/network/ui_network_setting.cpp \
	qt_ui/src/ui/setting/network/ui_network_setting_item_delegate.cpp \
	qt_ui/src/ui/setting/network/ui_wifi_list.cpp \
	qt_ui/src/ui/base/ui_proc_thread.cpp \
	qt_ui/src/ui/base/eth_management.cpp \
	qt_ui/src/ui/base/wifi_management.cpp \
	qt_ui/src/ui/base/ui_ota.cpp \
	qt_ui/src/ui/base/ui_rotate_management.cpp \
	qt_ui/src/ui/keyboard/ui_soft_keyboard.cpp \
	qt_ui/src/ui/keyboard/ui_soft_keyboard_context.cpp \
	qt_ui/src/ui/main/ui_face_scan.cpp \
	qt_ui/src/ui/main/ui_bind_qrcode.cpp \
	qt_ui/src/ui/main/ui_ble_qrcode.cpp \
	qt_ui/src/ui/main/ui_face_authentication.cpp \
	qt_ui/src/ui/setting/ui_password_login.cpp \
	qt_ui/src/ui/setting/ui_main_menu.cpp \
	qt_ui/src/ui/setting/factory/ui_factory_lcd.cpp \
	qt_ui/src/ui/setting/factory/ui_factory_touch.cpp \
	qt_ui/src/ui/setting/factory/ui_factory_camera.cpp \
	qt_ui/src/ui/setting/factory/ui_factory_wifi.cpp \
	qt_ui/src/ui/setting/factory/ui_factory_ble.cpp \
	qt_ui/src/ui/setting/factory/ui_factory_rtc.cpp \
	qt_ui/src/ui/setting/factory/ui_factory_eth.cpp \
	qt_ui/src/ui/setting/factory/ui_factory_range_sensor.cpp \
	qt_ui/src/ui/setting/factory/ui_factory_light_sensor.cpp \
	qt_ui/src/ui/setting/factory/ui_factory_light.cpp \
	qt_ui/src/ui/setting/factory/ui_factory_ir.cpp \
	qt_ui/src/ui/setting/factory/ui_factory_speaker.cpp \
	qt_ui/src/ui/setting/factory/ui_factory_main.cpp \
	qt_ui/src/ui/setting/factory/ui_camera_calibration.cpp \
	qt_ui/src/ui/setting/factory/ui_factory_relay.cpp \
	qt_ui/src/ui/setting/factory/ui_factory_rs485.cpp \
	qt_ui/src/ui/setting/factory/ui_factory_wiegand.cpp \
	qt_ui/src/ui/setting/factory/ui_factory_alarm.cpp \
	qt_ui/src/ui/setting/factory/ui_factory_usb.cpp \
	qt_ui/src/ui/setting/system/ui_system_setting.cpp \
	qt_ui/src/ui/setting/system/ui_system_maintenance_dialog.cpp \
	qt_ui/src/ui/setting/system/ui_main_setting.cpp \
	qt_ui/src/ui/setting/system/ui_system_setting_delegate.cpp \
	qt_ui/src/ui/setting/system/ui_modify_password.cpp \
	qt_ui/src/ui/setting/system/ui_modify_password_delegate.cpp \
	qt_ui/src/ui/setting/system/ui_aboat_storage.cpp \
	qt_ui/src/ui/setting/system/ui_setting_time.cpp \
	qt_ui/src/ui/setting/system/ui_zone_setting.cpp \
	qt_ui/src/ui/setting/system/ui_zone_setting_delegate.cpp \
	qt_ui/src/ui/setting/system/ui_volum_setting.cpp \
	qt_ui/src/ui/setting/system/ui_aboat_device_main.cpp \
	qt_ui/src/ui/setting/system/ui_aboat_device_main_delegate.cpp \
	qt_ui/src/ui/setting/system/ui_about_setting.cpp \
	qt_ui/src/ui/setting/system/ui_rtsp_setting.cpp \
	qt_ui/src/ui/setting/system/my_progressbar.cpp \
	qt_ui/src/ui/setting/door/ui_access_setting.cpp \
	qt_ui/src/ui/setting/door/ui_access_setting_item_delegate.cpp \
	qt_ui/src/ui/setting/door/ui_access_time_setting.cpp \
	qt_ui/src/ui/setting/system/ui_language_setting.cpp \
	qt_ui/src/ui/setting/checkin/ui_checkin_setting.cpp \
	qt_ui/src/ui/misc/ui_edit_dialog.cpp \
	qt_ui/src/ui/misc/ui_tip_dialog.cpp \
	qt_ui/src/ui/misc/ui_radio_dialog.cpp \
	qt_ui/src/ui/misc/ui_checkbox_dialog.cpp \
	qt_ui/src/ui/misc/ui_count_dialog.cpp \
	qt_ui/src/ui/misc/ui_time_dialog.cpp \
	qt_ui/src/ui/misc/ui_input_dialog.cpp \
	qt_ui/src/ui/misc/qtListView.cpp \
	qt_ui/src/ui/misc/ui_attendance_data_dialog.cpp \
	qt_ui/src/ui/misc/ui_user_search_dialog.cpp \
	qt_ui/src/ui/misc/clicked_label.cpp \
	qt_ui/src/ui/misc/roll_label.cpp \
	qt_ui/src/ui/screensaver/ui_wait_clock.cpp \
	qt_ui/src/ui/test/ui_test_for_draw.cpp \
	qt_ui/src/ui/test/widget.cpp \
	qt_ui/src/ui/test/ui_home_entering.cpp \
	$(QT_SRC_LIST) \
	qt_ui/src/ui/keyboard/googlepinyin/google_pinyin.cpp \
	qt_ui/src/ui/keyboard/googlepinyin/src/dictbuilder.cpp \
	qt_ui/src/ui/keyboard/googlepinyin/src/dictlist.cpp \
	qt_ui/src/ui/keyboard/googlepinyin/src/dicttrie.cpp \
	qt_ui/src/ui/keyboard/googlepinyin/src/lpicache.cpp \
	qt_ui/src/ui/keyboard/googlepinyin/src/matrixsearch.cpp \
	qt_ui/src/ui/keyboard/googlepinyin/src/mystdlib.cpp \
	qt_ui/src/ui/keyboard/googlepinyin/src/ngram.cpp \
	qt_ui/src/ui/keyboard/googlepinyin/src/pinyinime.cpp \
	qt_ui/src/ui/keyboard/googlepinyin/src/searchutility.cpp \
	qt_ui/src/ui/keyboard/googlepinyin/src/spellingtable.cpp \
	qt_ui/src/ui/keyboard/googlepinyin/src/spellingtrie.cpp \
	qt_ui/src/ui/keyboard/googlepinyin/src/splparser.cpp \
	qt_ui/src/ui/keyboard/googlepinyin/src/sync.cpp \
	qt_ui/src/ui/keyboard/googlepinyin/src/userdict.cpp \
	qt_ui/src/ui/keyboard/googlepinyin/src/utf16char.cpp \
	qt_ui/src/ui/keyboard/googlepinyin/src/utf16reader.cpp \
	qt_ui/src/ui/base/screen_size.cpp \
	qt_ui/src/ui/base/flowlayout.cpp \
	qt_ui/src/ui/base/ui_base_widget.cpp

FILE_INCLUDES += $(shell find external/live555/ -type d)
FILE_INCLUDES += $(shell find frameworks/media/ -type d)
LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/qt_ui/include \
	$(LOCAL_PATH)/qt_ui/qt_generate \
	$(LOCAL_PATH)/base/include \
	$(LOCAL_PATH)/api/include \
	$(LOCAL_PATH)/api/live_video/include \
	$(LOCAL_PATH)/api/live_video/src \
	${FILE_INCLUDES} \
	$(LOCAL_PATH)/face \
	$(LOCAL_PATH)/services/include \
	$(LOCAL_PATH)/wechat/include \
	$(LOCAL_PATH)/wechat/websocket/include \
	frameworks/qt4/include \
	frameworks/qt4/include/QtGui \
	frameworks/qt4/include/QtCore \
	frameworks/qt4/include/QtNetwork \
	frameworks/qt4/include/QtSql \
	frameworks/services/face/include \
	frameworks/services/face/src/ \
	frameworks/media/hal/include \
	frameworks/network/wifi/wifi_softap/lib \
	frameworks/network/wifi/wifi_station/lib \
	frameworks/network/net_utils/ \
	frameworks/network/ethernet/ \
	frameworks/network/smart_config/ent_wechat_ble/include \
	frameworks/media/jpeg/jpeg \
	frameworks/media/hal/include \
	frameworks/media/cut \
	frameworks/media/flip \
	frameworks/media/include \
	frameworks/media/openmax/oscl \
	frameworks/media/openmax/omx_base \
	frameworks/media/openmax/omx_base/inc \
	frameworks/media/openmax/component/arender_component \
	frameworks/media/openmax/component/vrender_component \
	frameworks/media/openmax/component/vrec_component \
	frameworks/media/openmax/api \
	frameworks/media/openmax/oscl/display \
	frameworks/media/recorder \
	frameworks/media/audio \
	frameworks/media/video \
	frameworks/media/mediaplayer \
	frameworks/tts/include \
	external/qrencode/qrencode_svc/inc \
	system/core/include/xlswriter \
	system/core/prebuilts/lib/curl/include \
	$(LOCAL_PATH)/qt_ui/src/ui/keyboard/googlepinyin \
	$(LOCAL_PATH)/qt_ui/src/ui/keyboard/googlepinyin/include \

# 注释此行如果你希望保留符合和调试信息
#LOCAL_STRIP_MODULE := false

# C编译器选项
LOCAL_CFLAGS := -Wall -Werror -DMG_ENABLE_THREADS -DMG_ENABLE_HTTP_STREAMING_MULTIPART -DMG_ENABLE_HTTP_WEBSOCKET=1 \
	-DMG_ENABLE_DEBUG=0 -DMG_ENABLE_HTTP=1 -DMG_ENABLE_SSL=1 -DNETWORK_DEBUG=1
# ATTENDANCE_MODE: 0,gate; 1,local; 2,wechat; 3:dintalk
LOCAL_CFLAGS += -DATTENDANCE_MODE=0
LOCAL_CFLAGS += -DDEF_WEBSERVER
ifeq ($(BOARD_SCREEN_ORIENTATION),portrait)
LOCAL_CFLAGS += -DPORTRAIT_SCREEN
endif
#-Wall -Werror
LOCAL_CFLAGS += -g3

# C++编译器选项
LOCAL_CPPFLAGS := -w -g3

# 依赖的静态库
LOCAL_STATIC_LIBRARIES := libgroupsock libliveMedia \
	libBasicUsageEnvironment libUsageEnvironment

# 链接器选项
# 如果需要引用sysroot中的库，请在此指定，例如引用pthread和math
# LOCAL_LDFLAGS := -lpthread -lm
$(foreach _ld, $(LOCAL_STATIC_LIBRARIES), \
        $(eval LD_PATH += -L$(TARGET_OUT)/../obj/STATIC_LIBRARIES/$(_ld)_intermediates) )

LOCAL_LDFLAGS := $(LD_PATH) -lpthread -lm -lts -lz -ljpeg -lpng -lfreetype -lssl -lcrypto -lopencv_core \
	-lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs -lopencv_videoio -lopencv_calib3d \
	-lopencv_features2d -lopencv_flann -lgroupsock -lliveMedia \
	-lBasicUsageEnvironment -lUsageEnvironment -ldl -lcurl


# 依赖的动态库
LOCAL_SHARED_LIBRARIES := libQtGui libQtCore libQtNetwork  libQtSql libface_svc libiniparser libxlsreader libxlsxwriter libxlswriter \
	liblombo_malloc libomx_arender librecorder libdrm libomx liblombo_jpeg libvideo libvideo libaudio libqrencode libflip libzbar \
	libnet_utils libethernet libwifi_station libwpa_client libwifi_softap libsys_sound libent_wechat libgattimpl \
	libdtbased_tk libnn_engine libopenvx libopenvx-nn libnn-priv libnn libsoft-nn liblog libcut libzbar \
	libtts-16k

include $(BUILD_EXECUTABLE)
