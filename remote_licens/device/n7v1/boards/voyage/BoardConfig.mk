TARGET_ARCH := arm
TARGET_ARCH_VARIANT := armv7-a-neon
TARGET_CPU_ABI := armeabi-v7a

BOARD_KERNEL_SRC := linux-3.10

ifeq ($(TARGET_PRODUCT),n7v1_voyage_uvc)
BOARD_KERNEL_DEFCONFIG := n7v1_voyage_uvc_defconfig
BOARD_DTB := n7v1-voyage_uvc.dtb
else
BOARD_KERNEL_DEFCONFIG := n7v1_voyage_defconfig
BOARD_DTB := n7v1-voyage.dtb
endif

BOARD_U_BOOT_LOAD_ADDR := 0x43000000
BOARD_PREBUILT_ROOTFS := rootfs.tar
BOARD_PREBUILT_SYSROOT := sysroot.tar
BOARD_BOOTFS_FORMAT := fat16
# size in sector
BOARD_BOOTFS_SIZE := 32768
BOARD_WIFI_VENDOR := broadcom
BOARD_WIFI_MODULE := ap6255

BOARD_ROOTFS_SIZE := 512M
