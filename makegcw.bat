#!/bin/bash
echo "Setting up GCW0 cross compiler"
export PATH=$PATH:/opt/gcw0-toolchain/usr/bin

echo "Creating build dir"
rm -rf buildgcw
mkdir buildgcw

echo "Copying source files"
cp -r $1/* buildgcw

echo "Removing old div exe"
rm "buildgcw/$2"

echo "Removing extra exe files"
find buildgcw -iname "*.exe" | xargs rm

echo "Removing dll files"
find buildgcw -iname "div*run.dll" | xargs rm

echo "Removing flic files"
find buildgcw -iname "*.fl*" | xargs rm

echo "Copying new exe files"
cp "$1/$2" "buildgcw/$3.dat"
echo "Creating desktop file"
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

VER=`dd if=buildgcw/$3.dat bs=1 count=1 skip=2 2>/dev/null`
#echo $VER

# s= div1 j=div2
if [ $VER = "j" ]
then
echo "DIV2 runtime"
cp system/divrun-GCW "buildgcw/$3"
else
echo "DIV1 runtime"
cp ./div1run-GCW "buildgcw/$3"
fi

echo "Compressing binary"
upx -9 "buildgcw/$3" > /dev/null

echo "Making opk file"

mksquashfs buildgcw $3.opk -all-root -noappend -no-exports -no-xattrs > /dev/null
#rm -rf buildgcw

echo "Done! - created $3.opk"

