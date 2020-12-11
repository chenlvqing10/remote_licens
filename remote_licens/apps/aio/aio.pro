QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXXFLAGS += -std=c++0x

DEFINES += QT_NO_WARNING_OUTPUT \
	QT_NO_DEBUG_OUTPUT

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

UI_DIR = ./qt_ui/qt_generate
RCC_DIR = ./qt_ui/qt_generate
MOC_DIR = ./qt_ui/qt_generate

SOURCES += \
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
	qt_ui/src/ui/setting/door/ui_access_setting.cpp \
	qt_ui/src/ui/setting/door/ui_access_time_setting.cpp \
	qt_ui/src/ui/setting/factory/ui_factory_alarm.cpp \
	qt_ui/src/ui/setting/system/ui_about_setting.cpp \
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
	qt_ui/src/ui/setting/checkin/ui_checkin_setting.cpp \
	qt_ui/src/ui/base/ui_proc_thread.cpp \
	qt_ui/src/ui/base/ui_ota.cpp \
	qt_ui/src/ui/base/eth_management.cpp \
	qt_ui/src/ui/base/wifi_management.cpp \
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
	qt_ui/src/ui/setting/factory/ui_factory_usb.cpp \
	qt_ui/src/ui/setting/system/ui_system_setting.cpp \
	qt_ui/src/ui/setting/system/ui_system_maintenance_dialog.cpp \
	qt_ui/src/ui/setting/system/ui_main_setting.cpp \
	qt_ui/src/ui/setting/system/ui_system_setting_delegate.cpp \
	qt_ui/src/ui/setting/system/ui_modify_password.cpp \
	qt_ui/src/ui/setting/system/ui_modify_password_delegate.cpp \
	qt_ui/src/ui/misc/ui_edit_dialog.cpp \
	qt_ui/src/ui/misc/ui_tip_dialog.cpp \
	qt_ui/src/ui/misc/ui_radio_dialog.cpp \
	qt_ui/src/ui/misc/ui_checkbox_dialog.cpp \
	qt_ui/src/ui/misc/ui_count_dialog.cpp \
	qt_ui/src/ui/misc/ui_time_dialog.cpp \
	qt_ui/src/ui/misc/ui_input_dialog.cpp \
	qt_ui/src/ui/misc/ui_attendance_data_dialog.cpp \
	qt_ui/src/ui/misc/clicked_label.cpp \
	qt_ui/src/ui/misc/qtListview.cpp \
	qt_ui/src/ui/misc/roll_label.cpp \
	qt_ui/src/ui/misc/ui_user_search_dialog.cpp \
	qt_ui/src/ui/screensaver/ui_wait_clock.cpp \
	qt_ui/src/ui/test/ui_test_for_draw.cpp \
	qt_ui/src/ui/test/widget.cpp \
	qt_ui/src/ui/test/ui_home_entering.cpp \
	qt_ui/src/ui/base/screen_size.cpp \
	qt_ui/src/ui/base/flowlayout.cpp \
	qt_ui/src/ui/base/ui_base_widget.cpp \
	qt_ui/src/ui/setting/system/ui_aboat_storage.cpp \
	qt_ui/src/ui/setting/system/ui_setting_time.cpp \
	qt_ui/src/ui/setting/system/ui_zone_setting.cpp \
	qt_ui/src/ui/setting/system/ui_zone_setting_delegate.cpp \
	qt_ui/src/ui/setting/system/ui_aboat_device_main.cpp \
	qt_ui/src/ui/setting/system/ui_aboat_device_main_delegate.cpp \
	qt_ui/src/ui/setting/system/ui_volum_setting.cpp \
	qt_ui/src/ui/setting/system/my_progressbar.cpp \
	qt_ui/src/ui/setting/system/ui_language_setting.cpp \
	qt_ui/src/ui/setting/system/ui_rtsp_setting.cpp

HEADERS += \
	qt_ui/include/QtMainWindow.hpp \
	qt_ui/include/QtDialog.hpp \
	qt_ui/include/signal_table.hpp \
	qt_ui/include/slot_table_test.hpp \
	qt_ui/include/biitem.hpp \
	qt_ui/include/checkin.hpp \
	qt_ui/include/exportcheckin.hpp \
	qt_ui/include/person.hpp \
	qt_ui/include/rule.hpp \
	qt_ui/include/sql_head.hpp \
	qt_ui/include/sqlapi.hpp \
	qt_ui/include/sqlbase.hpp \
	qt_ui/include/ui_about_setting.hpp \
	qt_ui/include/ui_access_setting.hpp \
	qt_ui/include/ui_access_time_setting.hpp \
	qt_ui/include/ui_factory_alarm.hpp \
	qt_ui/include/ui_user_search_dialog.hpp \
	qt_ui/include/widget.hpp \
	qt_ui/include/ui_home_entering.hpp \
	qt_ui/include/ui_wait_clock.hpp \
	qt_ui/include/ui_main_menu.hpp \
	qt_ui/include/ui_add_user.hpp \
	qt_ui/include/ui_add_user_delegate.hpp \
	qt_ui/include/ui_batch_import.hpp \
	qt_ui/include/ui_batch_register.hpp \
	qt_ui/include/ui_batch_recognition.hpp \
	qt_ui/include/ui_user_view_list.hpp \
	qt_ui/include/ui_wifi_list.hpp \
	qt_ui/include/ui_eth_setting.hpp \
	qt_ui/include/ui_eth_setting_delegate.hpp \
	qt_ui/include/ui_system_setting.hpp \
	qt_ui/include/ui_main_setting.hpp \
	qt_ui/include/ui_system_maintenance_dialog.hpp \
	qt_ui/include/ui_system_setting_delegate.hpp \
	qt_ui/include/ui_modify_password.hpp \
	qt_ui/include/ui_modify_password_delegate.hpp \
	qt_ui/include/ui_system_setting_delegate.hpp \
	qt_ui/include/ui_algorithm_setting.hpp \
	qt_ui/include/ui_algorithm_setting_delegate.hpp \
	qt_ui/include/ui_attendance_setting.hpp \
	qt_ui/include/ui_attendance_list.hpp \
	qt_ui/include/ui_attendance_data.hpp \
        qt_ui/include/ui_attendance_search.hpp \
	qt_ui/include/ui_setting_item_delegate.hpp \
	qt_ui/include/ui_face_scan.hpp \
	qt_ui/include/ui_password_login.hpp \
	qt_ui/include/ui_network_setting.hpp \
	qt_ui/include/ui_network_setting_item_delegate.hpp \
	qt_ui/include/ui_user_management.hpp \
	qt_ui/include/ui_user_management_delegate.hpp \
	qt_ui/include/ui_ota.hpp \
	qt_ui/include/eth_management.hpp \
	qt_ui/include/wifi_management.hpp \
	qt_ui/include/ui_rotate_management.hpp \
	qt_ui/include/ui_bind_qrcode.hpp \
	qt_ui/include/ui_test_for_draw.hpp \
	qt_ui/include/ui_soft_keyboard.hpp \
	qt_ui/include/ui_soft_keyboard_context.hpp \
	qt_ui/include/ui_face_authentication.hpp \
	qt_ui/include/ui_factory_main.hpp \
	qt_ui/include/ui_tip_dialog.hpp \
	qt_ui/include/ui_radio_dialog.hpp \
	qt_ui/include/ui_checkbox_dialog.hpp \
	qt_ui/include/ui_count_dialog.hpp \
	qt_ui/include/ui_time_dialog.hpp \
	qt_ui/include/ui_input_dialog.hpp \
	qt_ui/include/ui_attendance_data_dialog.hpp \
	qt_ui/include/ui_factory_lcd.hpp \
	qt_ui/include/ui_factory_touch.hpp \
	qt_ui/include/ui_factory_camera.hpp \
	qt_ui/include/ui_factory_wifi.hpp \
	qt_ui/include/ui_factory_ble.hpp \
	qt_ui/include/ui_factory_rtc.hpp \
	qt_ui/include/ui_factory_eth.hpp \
	qt_ui/include/ui_factory_range_sensor.hpp \
	qt_ui/include/ui_factory_light_sensor.hpp \
	qt_ui/include/ui_factory_light.hpp \
	qt_ui/include/ui_factory_ir.hpp \
	qt_ui/include/ui_factory_speaker.hpp \
	qt_ui/include/ui_factory_relay.hpp \
	qt_ui/include/ui_factory_rs485.hpp \
	qt_ui/include/ui_factory_wiegand.hpp \
	qt_ui/include/ui_factory_usb.hpp \
	qt_ui/include/roll_label.hpp \
	qt_ui/include/clicked_label.hpp \
	qt_ui/include/qtListView.hpp \
	qt_ui/include/ui_debug_switch.hpp \
	qt_ui/include/ui_debug_switch_delegate.hpp \
	qt_ui/include/my_mouse.hpp \
	qt_ui/include/ui_proc_thread.hpp \
	qt_ui/include/ui_ble_qrcode.hpp \
	qt_ui/include/ui_camera_calibration.hpp \
	qt_ui/include/ui_edit_dialog.hpp \
	qt_ui/include/screen_size.hpp \
	qt_ui/include/flowlayout.hpp \
	qt_ui/include/ui_aboat_storage.hpp \
	qt_ui/include/ui_base_widget.hpp \
	qt_ui/include/ui_setting_time.hpp \
	qt_ui/include/ui_zone_setting.hpp \
	qt_ui/include/ui_zone_setting_delegate.hpp \
	qt_ui/include/ui_volum_setting.hpp \
	qt_ui/include/ui_aboat_device_main.hpp \
	qt_ui/include/ui_aboat_device_main_delegate.hpp \
	qt_ui/include/my_progressbar.hpp \
	qt_ui/include/ui_language_setting.hpp \
	qt_ui/include/ui_rtsp_setting.hpp \
	qt_ui/include/ui_checkin_setting.hpp

FORMS += \
	qt_ui/layout/keyboard/ui_soft_keyboard.ui \
	qt_ui/layout/main/ui_bind_qrcode.ui \
	qt_ui/layout/main/ui_ble_qrcode.ui \
	qt_ui/layout/main/ui_face_authentication.ui \
	qt_ui/layout/main/ui_face_scan.ui \
	qt_ui/layout/main/ui_face_scan_v.ui \
	qt_ui/layout/misc/ui_edit_dialog.ui \
	qt_ui/layout/misc/ui_tip_dialog.ui \
	qt_ui/layout/misc/ui_radio_dialog.ui \
	qt_ui/layout/misc/ui_count_dialog.ui \
	qt_ui/layout/misc/ui_time_dialog.ui \
	qt_ui/layout/misc/ui_input_dialog.ui \
	qt_ui/layout/misc/ui_attendance_data_dialog.ui \
	qt_ui/layout/misc/ui_user_search_dialog.ui \
	qt_ui/layout/screensaver/ui_wait_clock.ui \
	qt_ui/layout/settings/algorithm/ui_algorithm_setting.ui \
	qt_ui/layout/settings/attendance/ui_attendance_setting.ui \
	qt_ui/layout/settings/attendance/ui_attendance_list.ui \
	qt_ui/layout/settings/attendance/ui_attendance_data.ui \
	qt_ui/layout/settings/attendance/ui_attendance_search.ui \
	qt_ui/layout/settings/debug/ui_batch_import.ui \
	qt_ui/layout/settings/debug/ui_batch_recognition.ui \
	qt_ui/layout/settings/debug/ui_batch_register.ui \
	qt_ui/layout/settings/debug/ui_debug_switch.ui \
	qt_ui/layout/settings/door/ui_access_setting.ui \
	qt_ui/layout/settings/door/ui_access_time_setting.ui \
	qt_ui/layout/settings/factory/ui_camera_calibration.ui \
	qt_ui/layout/settings/factory/ui_factory_alarm.ui \
	qt_ui/layout/settings/factory/ui_factory_ble.ui \
	qt_ui/layout/settings/factory/ui_factory_camera.ui \
	qt_ui/layout/settings/factory/ui_factory_ir.ui \
	qt_ui/layout/settings/factory/ui_factory_lcd.ui \
	qt_ui/layout/settings/factory/ui_factory_light.ui \
	qt_ui/layout/settings/factory/ui_factory_light_sensor.ui \
	qt_ui/layout/settings/factory/ui_factory_main.ui \
	qt_ui/layout/settings/factory/ui_factory_eth.ui \
	qt_ui/layout/settings/factory/ui_factory_range_sensor.ui \
	qt_ui/layout/settings/factory/ui_factory_rtc.ui \
	qt_ui/layout/settings/factory/ui_factory_speaker.ui \
	qt_ui/layout/settings/factory/ui_factory_touch.ui \
	qt_ui/layout/settings/factory/ui_factory_wifi.ui \
	qt_ui/layout/settings/factory/ui_factory_relay.ui \
	qt_ui/layout/settings/factory/ui_factory_rs485.ui \
	qt_ui/layout/settings/factory/ui_factory_wiegand.ui \
	qt_ui/layout/settings/factory/ui_factory_usb.ui \
	qt_ui/layout/settings/network/ui_eth_setting.ui \
	qt_ui/layout/settings/network/ui_network_setting.ui \
	qt_ui/layout/settings/network/ui_wifi_list.ui \
	qt_ui/layout/settings/system/ui_aboat_storage.ui \
	qt_ui/layout/settings/system/ui_about_setting.ui \
	qt_ui/layout/settings/system/ui_system_setting.ui \
	qt_ui/layout/settings/system/ui_system_maintenance_dialog.ui \
	qt_ui/layout/settings/system/ui_main_setting.ui \
	qt_ui/layout/settings/system/ui_modify_password.ui \
	qt_ui/layout/settings/system/ui_setting_time.ui \
	qt_ui/layout/settings/system/ui_zone_setting.ui \
	qt_ui/layout/settings/system/ui_rtsp_setting.ui \
	qt_ui/layout/settings/user/ui_add_user.ui \
	qt_ui/layout/settings/user/ui_user_management.ui \
	qt_ui/layout/settings/user/ui_user_view_list.ui \
	qt_ui/layout/settings/checkin/ui_checkin_setting.ui \
	qt_ui/layout/settings/ui_main_menu.ui \
	qt_ui/layout/settings/ui_main_menu_wechat.ui \
	qt_ui/layout/settings/ui_password_login.ui \
	qt_ui/layout/test/ui_home_entering.ui \
	qt_ui/layout/test/ui_identify_authentication.ui \
	qt_ui/layout/test/ui_test_for_draw.ui \
	qt_ui/layout/test/widget.ui

RESOURCES += \
	qt_ui/res/icons/icon.qrc

TRANSLATIONS += \
	qt_ui/res/icons/setting/qt_gui_en_US.ts \
	qt_ui/res/icons/setting/qt_gui_zh_CN.ts

include($$PWD/qt_ui/src/ui/keyboard/googlepinyin/googlepinyin.pri)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

