#!/bin/bash

# This script is used for building QT flies and generate standard c++ files to specify directory
# Virgo Build System will build files in specify directory

export PATH=$VIRGO_BUILD_TOP/frameworks/qt4/bin:$PATH

# Current directory is build top
cd apps/aio && qmake

# The newest target
MOC=`grep -E "^qt_ui/qt_generate/moc_" Makefile | awk -F ':' '{print $1}'`
UI=`grep -E "^qt_ui/qt_generate/ui_" Makefile | awk -F ':' '{print $1}'`
QRC=`grep -E "^qt_ui/qt_generate/qrc_" Makefile | awk -F ':' '{print $1}'`

# Old target
OLD_MOC=`find qt_ui/qt_generate/ -name "moc_*" | awk -F ':' '{print $1}'`
OLD_UI=`find qt_ui/qt_generate/ -name "ui*" | awk -F ':' '{print $1}'`
OLD_QRC=`find qt_ui/qt_generate/ -name "qrc_*" | awk -F ':' '{print $1}'`

# Delete files outside of newest target
OLD_IFS="$IFS"
IFS=" "
ARR_MOC=$OLD_MOC
ARR_UI=$OLD_UI
ARR_QRC=$OLD_QRC
IFS="$OLD_IFS"

for s in ${ARR_MOC[@]}
do
if [[ $MOC != *$s* ]];
then
	rm $s
fi
done

for s in ${ARR_UI[@]}
do
if [[ $UI != *$s* ]];
then
	rm $s
fi
done

for s in ${ARR_QRC[@]}
do
if [[ $QRC != *$s* ]];
then
	rm $s
fi
done

# Make newest target
if [ -n "$MOC" ]
then
	make $MOC
fi

if [ -n "$UI" ]
then
	make $UI
fi

if [ -n "$QRC" ]
then
	make $QRC
fi

cd ../../
