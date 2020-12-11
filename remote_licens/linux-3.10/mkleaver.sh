#!/bin/bash
set -e

if [ "x$1" == "x" ]; then
	echo -e "\033[;31m miss cpu\033[0m"
	exit 1
fi

if [ "x$2" == "x" ]; then
	echo -e "\033[;31m miss platform\033[0m"
	exit 1
fi

if [ "x$3" == "x" ]; then
	echo -e "\033[;31m miss board\033[0m"
	exit 1
fi

LOMBO_CPU=$1
LOMBO_PLATFORM=$2
LOMBO_BOARD=$3

LEAVER_NODTB_IMG=leaver_boot-nodtb.img

if [ "$LOMBO_CPU" == "n7" ]; then
	DST_DTB=arch/arm/boot/dts/$LOMBO_CPU$LOMBO_PLATFORM.dtb
elif [ "$LOMBO_CPU" == "n9" ]; then
	DST_DTB=arch/arm64/boot/dts/$LOMBO_CPU$LOMBO_PLATFORM-$LOMBO_BOARD.dtb
fi

function create_leaver_img()
{
	cat $LEAVER_NODTB_IMG $DST_DTB > leaver_boot-dtb.img
	mkimage -A arm -O u-boot -T firmware -C none -a 0x43000000 -e 0 -n "U-Boot 2016.11 for lombo board" -z leaver -d leaver_boot-dtb.img leaver_boot.img
}

if [ ! -f $LEAVER_NODTB_IMG ]; then
	echo "there is no $LEAVER_NODTB_IMG"
	exit 1
fi

if [ ! -f $DST_DTB ]; then
	echo "there is no $DST_DTB"
	exit 1
fi

create_leaver_img
exit 0
