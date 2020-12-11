# Files for pack tool
PRODUCT_COPY_FILES += \
	device/n7v1/pack/hw_scan.bin:pack/hw_scan.bin \
	device/n7v1/pack/M0024.dll:pack/M0024.dll \
	device/n7v1/pack/M0036.dll:pack/M0036.dll \
	device/n7v1/pack/mmc_fmt.bin:pack/mmc_fmt.bin \
	device/n7v1/pack/nand_fmt.bin:pack/nand_fmt.bin \
	device/n7v1/pack/plug.cfg:pack/plug.cfg \
	device/n7v1/pack/pod_mmc.bin:pack/pod_mmc.bin \
	device/n7v1/pack/pod_nand.bin:pack/pod_nand.bin \
	device/n7v1/pack/pod_spi.bin:pack/pod_spi.bin \
	device/n7v1/pack/pom_init.bin:pack/pom_init.bin \
	device/n7v1/pack/pom_main.bin:pack/pom_main.bin \
	device/n7v1/pack/pot_reset.bin:pack/pot_reset.bin \
	device/n7v1/pack/spi_fmt.bin:pack/spi_fmt.bin \
	device/n7v1/pack/spnd_cap.bin:pack/spnd_cap.bin \
	device/n7v1/pack/spnd_libs.bin:pack/spnd_libs.bin

# Bootloade files
PRODUCT_COPY_FILES += \
	device/n7v1/bootloader/bootfs.img:pack/bootfs.img \
	device/n7v1/bootloader/booster_raw.img:pack/booster_raw.img \
	device/n7v1/bootloader/param.cfg:pack/param.cfg \
	device/n7v1/bootloader/leaver_boot-nodtb.img:pack/leaver_boot-nodtb.img \
	device/n7v1/boards/p1/bootloader/leaver.ini:pack/leaver.ini \
	device/n7v1/boards/p1/bootloader/env.cfg:pack/env.cfg \
	device/n7v1/bootloader/resume.bin:pack/resume.bin

# MBR and firmware configuration
PRODUCT_COPY_FILES += \
	device/n7v1/boards/p1/configs/imbr_mmc.ini:pack/imbr_mmc.ini \
	device/n7v1/boards/p1/configs/imbr_nand.ini:pack/imbr_nand.ini \
	device/n7v1/boards/p1/configs/imbr_nor.ini:pack/imbr_nor.ini \
	device/n7v1/boards/p1/configs/imbr_spi_nand.ini:pack/imbr_spi_nand.ini \
	device/n7v1/boards/p1/configs/pack.ini:pack/pack.ini \
	device/n7v1/boards/p1/configs/update.ini:pack/update.ini \
	device/n7v1/boards/p1/configs/update.cfg:pack/update.cfg

# bootfs
PRODUCT_COPY_FILES += \
	device/n7v1/boards/p1/bootfs/res/boot.bmp:bootfs/res/boot.bmp \
	device/n7v1/boards/p1/bootfs/res/efont.ttf:bootfs/res/efont.ttf

# GUI configuration
#-include frameworks/gui/gui-armhf.mk
-include frameworks/qt4/qt-armhf.mk
-include apps/aio/qt_ui/res/res.mk

# nn model files
-include frameworks/nn/lib/sv810/model.mk

# tts
#-include frameworks/tts/8k/data/model.mk
-include frameworks/tts/16k/data/model.mk

# face-recg http files
-include apps/face-recg/http/http.mk

# rootfs
PRODUCT_FORCE_COPY_FILES += \
	device/n7v1/boards/p1/rootfs/etc/mdev.conf:rootfs/etc/mdev.conf \
	device/n7v1/boards/p1/rootfs/etc/inittab:rootfs/etc/inittab \
	device/n7v1/boards/p1/rootfs/etc/profile:rootfs/etc/profile

PRODUCT_COPY_FILES += \
	device/n7v1/boards/p1/rootfs/etc/init.d/S15factory:rootfs/etc/init.d/S15factory \
	device/n7v1/boards/p1/rootfs/etc/init.d/S20mount:rootfs/etc/init.d/S20mount \
	device/n7v1/boards/p1/rootfs/etc/init.d/S99cpufreq_governor_switch:rootfs/etc/init.d/S99cpufreq_governor_switch \
	device/n7v1/boards/p1/rootfs/etc/hotplug/udisk_insert:rootfs/etc/hotplug/udisk_insert \
	device/n7v1/boards/p1/rootfs/etc/hotplug/udisk_insert_s:rootfs/etc/hotplug/udisk_insert_s \
	device/n7v1/boards/p1/rootfs/etc/hotplug/udisk_remove:rootfs/etc/hotplug/udisk_remove \
	device/n7v1/boards/p1/rootfs/etc/hotplug/sdcard_insert:rootfs/etc/hotplug/sdcard_insert \
	device/n7v1/boards/p1/rootfs/etc/hotplug/sdcard_insert_s:rootfs/etc/hotplug/sdcard_insert_s \
	device/n7v1/boards/p1/rootfs/etc/hotplug/sdcard_remove:rootfs/etc/hotplug/sdcard_remove

# viss_isp configuration
PRODUCT_COPY_FILES += \
	device/n7v1/boards/p1/rootfs/etc/init.d/S50viss_isp:rootfs/etc/init.d/S50viss_isp

PRODUCT_COPY_DIRS += \
	device/n7v1/boards/p1/rootfs/etc/isp:rootfs/etc/isp

# face-recg
PRODUCT_COPY_FILES += \
	device/n7v1/boards/p1/rootfs/etc/face-recg.conf:rootfs/etc/face-recg.conf \
	device/n7v1/boards/p1/rootfs/etc/init.d/S60face-recg:rootfs/etc/init.d/S60face-recg

# face-recg resource
#-include apps/face-recg/res/res.mk

# face service configuration
PRODUCT_COPY_FILES += \
	device/n7v1/boards/p1/rootfs/etc/factory-test.ini:rootfs/etc/factory-test.ini \
	device/n7v1/boards/p1/rootfs/etc/face_svc.conf:rootfs/etc/face_svc.conf \
	device/n7v1/boards/p1/rootfs/etc/cal_params:rootfs/etc/cal_params \
	device/n7v1/boards/p1/rootfs/etc/cal_params.0:rootfs/etc/cal_params.0 \
	device/n7v1/boards/p1/rootfs/etc/map_params:rootfs/etc/map_params

# alsalib configuration
PRODUCT_COPY_FILES += \
	device/n7v1/boards/p1/rootfs/etc/asound.conf:rootfs/etc/asound.conf

# Add product packages here
PRODUCT_PACKAGES += \
	aio \
	uart_virtualtp
# USB device
PRODUCT_FORCE_COPY_FILES += \
	device/n7v1/boards/p1/rootfs/etc/u-disk-script:rootfs/etc/u-disk-script

# wifi configuration
-include frameworks/network/wifi/wifi_station/lib/wifi_station_base.mk
-include frameworks/network/wifi/wifi_softap/lib/wifi_softap_base.mk
-include frameworks/network/wifi/common/wifi_common.mk

PRODUCT_PACKAGES += \
	wpa_supplicant \
	wpa_cli \
	libwpa_client \
	hostapd \
	hostapd_cli \
	sta_connect \
	sta_disconnect \
	sta_scan

PRODUCT_NAME := n7v1_p1
PRODUCT_DEVICE := p1
PRODUCT_BRAND := Virgo
PRODUCT_MODEL := Virgo on n7v1
PRODUCT_CHIP := n7v1
PRODUCT_TOOLS := gcc-linaro-5.5.0-2017.10-x86_64_arm-linux-gnueabihf
PRODUCT_TOOLS_ARCH := armhf
PRODUCT_TOOLS_PREFIX := arm-linux-gnueabihf-
