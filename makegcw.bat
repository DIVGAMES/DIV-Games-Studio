#!/bin/bash

rm -rf buildgcw
mkdir buildgcw
cp -r $1/* buildgcw
rm buildgcw/*.exe > /dev/null
rm buildgcw/*.EXE > /dev/null
rm buildgcw/div*run.* > /dev/null
rm buildgcw/DIV*RUN.* > /dev/null

cp $1/$2 buildgcw/$3.dat
cat << EOF > buildgcw/default.gcw0.desktop
[Desktop Entry]

Type=Game
Name=$4
Comment=Made with DIV Games studio
Exec=$3 $3.dat
Icon=$3
Categories=games;
Terminal=false
X-OD-NeedsDownscaling=true
X-OD-NeedsJoystick=false

EOF

#cat buildgcw/default.gcw0.desktop



echo "Making $4 GCW BINARY"
cmake . -DGCW=1 -DWINDOWS=0 -DAMIGA=0 -DRPI=0 > /dev/null
make -j5 div1run-GCW divrun-GCW > /dev/null

echo "Copying $3 to buildgcw/$3"

if [ -z "$4" ]
then
echo "DIV2 runtime"
cp system/divrun-GCW "buildgcw/$3"
else
echo "DIV1 runtime"
cp ./div1run-GCW "buildgcw/$3"
fi

echo "Compressing binary"
upx -9 "buildgcw/$3" > /dev/null

#make -f Makefile.html dirs R1EMBED="--preload-file $1@" D1HTML_EXE="$2" DIV1RUN=html/$3.js html/$3.js

echo "Making opk file"

mksquashfs buildgcw $3.opk -all-root -noappend -no-exports -no-xattrs > /dev/null
rm -rf buildgcw

echo "Done! - created $3.opk"

