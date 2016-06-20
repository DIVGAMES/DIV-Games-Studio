#!/bin/bash
echo "Setting up GCW0 cross compiler"
export PATH=$PATH:/opt/gcw0-toolchain/usr/bin

TARGET=/tmp/buildgcw

echo "Creating build dir"
rm -rf ${TARGET}
mkdir -p ${TARGET}

echo "Copying source files to ${TARGET}"
cp -r $1/* ${TARGET}

cp tools/dx.png ${TARGET}/$3.png

echo "Removing extra exe files"
find ${TARGET} -iname "*.exe" | xargs rm 2>/dev/null

echo "Removing dll files"
find ${TARGET} -iname "div*run.dll" | xargs rm 2>/dev/null

echo "Removing flic files"
find ${TARGET} -iname "*.fl*" | xargs rm 2>/dev/null

echo "Copying new exe files"
cp "$1/$2" "${TARGET}/$3.dat"
echo "Creating desktop file"
cat << EOF > ${TARGET}/default.gcw0.desktop
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

echo "Making $4 GCW BINARY"
VER=`dd if=${TARGET}/$3.dat bs=1 count=1 skip=2 2>/dev/null`
#echo $VER

mkdir -p bgcw/system
cd bgcw
cmake .. -DTARGETOS=GCW > /dev/null

make -j5 div1run-GCW divrun-GCW > /dev/null
mipsel-linux-strip system/*-GCW

echo "Copying $3 to buildgcw/$3"


# s= div1 j=div2
if [ $VER = "s" ]
then
echo "DIV1 runtime"
cp ./div1run-GCW "${TARGET}/$3"
else
echo "DIV2 runtime"
cp system/divrun-GCW "${TARGET}/$3"
fi

cd ..

echo "Compressing binary"
upx -9 "${TARGET}/$3" > /dev/null

echo "Making opk file $3.opk"

mksquashfs ${TARGET} $3.opk -all-root -noappend -no-exports -no-xattrs > /dev/null
#rm -rf ${TARGET}

echo "Done! - created $3.opk"

