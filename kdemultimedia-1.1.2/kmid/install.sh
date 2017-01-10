#!/bin/sh
# This script installs kmid just following some fixed steps,
# the best (ideal) way to install kmid is by running 'su -c "make install"'
# but anyway, here is a small script that may or may not work.
# To use it, you have to first chmod this file to add execute permissions

echo Installing kmid using $KDEDIR as kde top level directory
echo Preparing Directories ...
mkdir -p $KDEDIR/share/apps/kmid
mkdir -p $KDEDIR/share/apps/kmid/maps
mkdir -p $KDEDIR/share/apps/kmid/toolbar
mkdir -p $KDEDIR/share/doc/HTML/default/kmid
mkdir -p $KDEDIR/share/applnk/Multimedia
echo Installing Binary...
cp kmid $KDEDIR/bin/kmid
echo Installing Menu Entry ...
cp kmid.kdelnk $KDEDIR/share/applnk/Multimedia/kmid.kdelnk
echo Installing Mimetype Entry for audio/x-karaoke ...
cp x-karaoke.kdelnk $KDEDIR/share/mimelnk/audio/x-karaoke.kdelnk
echo Installing Help...
cp kmid.html $KDEDIR/share/doc/HTML/default/kmid/kmid.html
echo Installing Icon...
cp kmid.xpm $KDEDIR/share/icons/kmid.xpm
echo Installing Toolbar Icons ...
cd toolbarxpm
cp *.xpm $KDEDIR/share/apps/kmid/toolbar
cd ..
echo Installing Midi Mappers ...
cd maps
cp *.map $KDEDIR/share/apps/kmid/maps
cd ..
echo Installing Examples Songs in $KDEDIR/share/apps/kmid ...
cd examples
cp *.kar $KDEDIR/share/apps/kmid
cd ..
