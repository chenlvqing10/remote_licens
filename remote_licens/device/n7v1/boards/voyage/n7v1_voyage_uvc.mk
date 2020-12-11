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
	device/n7v1/boards/voyage/bootloader/leaver.ini:pack/leaver.ini \
	device/n7v1/boards/voyage/bootloader/env.cfg:pack/env.cfg \
	device/n7v1/boards/voyage/bootloader/resume.bin:pack/resume.bin

# MBR and firmware configuration
PRODUCT_COPY_FILES += \
	device/n7v1/boards/voyage/configs/imbr_mmc.ini:pack/imbr_mmc.ini \
	device/n7v1/boards/voyage/configs/imbr_nand.ini:pack/imbr_nand.ini \
	device/n7v1/boards/voyage/configs/imbr_nor.ini:pack/imbr_nor.ini \
	device/n7v1/boards/voyage/configs/imbr_spi_nand.ini:pack/imbr_spi_nand.ini \
	device/n7v1/boards/voyage/configs/pack.ini:pack/pack.ini \
	device/n7v1/boards/voyage/configs/update.ini:pack/update.ini \
	device/n7v1/boards/voyage/configs/update.cfg:pack/update.cfg

# bootfs
PRODUCT_COPY_FILES += \
	device/n7v1/boards/voyage/bootfs/res/boot.bmp:bootfs/res/boot.bmp

# nn model files
-include frameworks/nn/lib/sv810/model.mk

# rootfs
PRODUCT_FORCE_COPY_FILES += \
	device/n7v1/boards/voyage/rootfs/etc/mdev.conf:rootfs/etc/mdev.conf \
	device/n7v1/boards/voyage/rootfs/etc/inittab:rootfs/etc/inittab

PRODUCT_COPY_FILES += \
	device/n7v1/boards/voyage/rootfs/etc/init.d/S20mount:rootfs/etc/init.d/S20mount \
	device/n7v1/boards/voyage/rootfs/etc/init.d/S99cpufreq_governor_switch:rootfs/etc/init.d/S99cpufreq_governor_switch \
	device/n7v1/boards/voyage/rootfs/etc/hotplug/udisk_insert:rootfs/etc/hotplug/udisk_insert \
	device/n7v1/boards/voyage/rootfs/etc/hotplug/udisk_insert_s:rootfs/etc/hotplug/udisk_insert_s \
	device/n7v1/boards/voyage/rootfs/etc/hotplug/udisk_remove:rootfs/etc/hotplug/udisk_remove \
	device/n7v1/boards/voyage/rootfs/etc/hotplug/sdcard_insert:rootfs/etc/hotplug/sdcard_insert \
	device/n7v1/boards/voyage/rootfs/etc/hotplug/sdcard_insert_s:rootfs/etc/hotplug/sdcard_insert_s \
	device/n7v1/boards/voyage/rootfs/etc/hotplug/sdcard_remove:rootfs/etc/hotplug/sdcard_remove

# viss_isp configuration
PRODUCT_COPY_FILES += \
	device/n7v1/boards/voyage/rootfs/etc/init.d/S50viss_isp:rootfs/etc/init.d/S50viss_isp

PRODUCT_COPY_DIRS += \
	device/n7v1/boards/voyage/rootfs/etc/isp:rootfs/etc/isp

# uvc-face-recg resource
PRODUCT_COPY_DIRS += \
	apps/voyage-demo/uvc-face-recg/res/db/face-uvc.db:rootfs/etc/face-uvc.db \
	apps/voyage-demo/uvc-face-recg/res/db/record-uvc.db:rootfs/etc/record-uvc.db \
	apps/voyage-demo/uvc-face-recg/res/db/config-uvc.db:rootfs/etc/config-uvc.db \
	device/n7v1/boards/voyage/rootfs/etc/face-recg.conf:rootfs/etc/face-recg.conf \
	device/n7v1/boards/voyage/rootfs/etc/init.d/S70uvc-face-recg:rootfs/etc/init.d/S70uvc-face-recg \
	apps/voyage-demo/uvc-face-recg/res/elang/elang.el:rootfs/usr/share/uvc-face-recg/res/elang/elang.el \
	apps/voyage-demo/uvc-face-recg/res/img/1.rgb:rootfs/usr/share/uvc-face-recg/res/img/1.rgb \
	apps/voyage-demo/uvc-face-recg/res/img/2.rgb:rootfs/usr/share/uvc-face-recg/res/img/2.rgb \
	apps/voyage-demo/uvc-face-recg/res/img/3.rgb:rootfs/usr/share/uvc-face-recg/res/img/3.rgb \
	apps/voyage-demo/uvc-face-recg/res/img/4.rgb:rootfs/usr/share/uvc-face-recg/res/img/4.rgb \
	apps/voyage-demo/uvc-face-recg/res/img/5.rgb:rootfs/usr/share/uvc-face-recg/res/img/5.rgb \
	apps/voyage-demo/uvc-face-recg/res/img/6.rgb:rootfs/usr/share/uvc-face-recg/res/img/6.rgb \

# service configuration
PRODUCT_COPY_FILES += \
	device/n7v1/boards/voyage/rootfs/etc/face_svc.conf:rootfs/etc/face_svc.conf \
	device/n7v1/boards/voyage/rootfs/etc/cal_params:rootfs/etc/cal_params \
	device/n7v1/boards/voyage/rootfs/etc/uvc_face_svc.conf:rootfs/etc/uvc_face_svc.conf

# alsalib configuration
PRODUCT_COPY_FILES += \
	device/n7v1/boards/voyage/rootfs/etc/asound.conf:rootfs/etc/asound.conf

# Add product test case packages here
PRODUCT_PACKAGES += \
	modetest \
	video_preview \
	media_server \
	lbrecorder \
	recorder_show_components \
	auto_lb \
	omxtest \
	omxplayer_test \
	lombo_get_sn \
	lombo_get_chipid\
	jpeg_enc_test\
	jpeg_enc_test_nv12\
	video_recorder

# Add product app demos packages here
PRODUCT_PACKAGES += \
	uvc-gadget \
	uvc-face-recg \
	uvc-gadget2

# USB device
PRODUCT_FORCE_COPY_FILES += \
	device/n7v1/boards/voyage/rootfs/etc/u-disk-script:rootfs/etc/u-disk-script

BUILD_ID := Virgo
BUILD_NUMBER := v1.6
PRODUCT_NAME := n7v1_voyage_uvc
PRODUCT_DEVICE := voyage
PRODUCT_BRAND := Virgo
PRODUCT_MODEL := Virgo on n7v1
PRODUCT_CHIP := n7v1
PRODUCT_TOOLS := gcc-linaro-5.5.0-2017.10-x86_64_arm-linux-gnueabihf
PRODUCT_TOOLS_ARCH := armhf
PRODUCT_TOOLS_PREFIX := arm-linux-gnueabihf-
