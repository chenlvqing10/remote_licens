TARGET_ARCH := arm
TARGET_ARCH_VARIANT := armv7-a-neon
TARGET_CPU_ABI := armeabi-v7a

BOARD_KERNEL_SRC := linux-3.10
BOARD_KERNEL_DEFCONFIG := n7v1_p1s_defconfig
BOARD_DTB := n7v1-p1s.dtb
BOARD_U_BOOT_LOAD_ADDR := 0x43000000
BOARD_PREBUILT_ROOTFS := rootfs.tar
BOARD_PREBUILT_SYSROOT := sysroot.tar
BOARD_BOOTFS_FORMAT := fat16
# size in sector
BOARD_BOOTFS_SIZE := 32768
BOARD_WIFI_VENDOR := realtek
BOARD_WIFI_MODULE := rtl8189ftv
# landscape / portrait, landscape default ui
BOARD_SCREEN_ORIENTATION := portrait
