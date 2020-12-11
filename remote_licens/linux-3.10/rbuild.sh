#!/bin/bash
set -e

ramfs_dir=ramfs
ramfs_name=initramfs_data.cpio.gz

function show_help()
{
	echo -e ""
	echo -e "-------------------------------To create ramfs as follow--------------------------"
	echo -e "1. run \033[;31m./rbuild.sh x $ramfs_name\033[0m to extract ramfs to the $ramfs_dir folder"
	echo -e "2. make changes in the $ramfs_dir folder"
	echo -e "3. run \033[;31m./rbuild.sh c $ramfs_name\033[0m to create ramfs from the $ramfs_dir folder"
}

function extract_ramfs()
{
	if [ -f $1 ]; then
		if [ -d $ramfs_dir ]; then
			rm -fr $ramfs_dir
		fi

		mkdir $ramfs_dir
		cp $ramfs_name $ramfs_dir/
		cd $ramfs_dir/
		gunzip $ramfs_name
		cpio -idmv < initramfs_data.cpio > /dev/null 2>&1
		rm initramfs_data.cpio
	else
		echo "$1 not exist"
		exit 1
	fi
}

function create_ramfs()
{
	if [ -d $ramfs_dir ]; then
		(cd $ramfs_dir;find . | fakeroot cpio -o -Hnewc | gzip > ../$1)
	else
		echo "$ramfs_dir not exist, run ./rbuild.sh x $ramfs_name first"
		exit 1
	fi
}

if [ $# -ne 2 ]; then
	show_help
	exit 1
fi

if [ "$1" == "x" ]; then
	extract_ramfs $2
elif [ "$1" == "c" ]; then
	create_ramfs $2
else
	show_help
	exit 1
fi
