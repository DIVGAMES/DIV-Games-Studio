#!/bin/bash

if [ -z $5 ]
then
BUILDS="JPRGLSWD"
else
BUILDS=$5
fi

echo $BUILDS

HTMLBUILD=`expr match $BUILDS '.*J.*'`
GCWBUILD=`expr match $BUILDS '.*G.*'`
DROIDBUILD=`expr match $BUILDS '.*A.*'`
PANDORABUILD=`expr match $BUILDS '.*P.*'`
PIBUILD=`expr match $BUILDS '.*R.*'`
WINDOWSBUILD=`expr match $BUILDS '.*W.*'`
PSPBUILD=`expr match $BUILDS '.*S.*'`
LINUXBUILD=`expr match $BUILDS '.*L.*'`


rm -rf zipdiv
mkdir zipdiv
cd zipdiv
echo "unzipping file"

unzip -j $1 >/dev/null

echo "converting files to lowercase"

find . -depth -exec rename 's/(.*)\/([^\/]*)/$1\/\L$2/' {} \;

if [ -f setup.exe ]
then
rm setup.exe 
fi

EXE=`ls *.exe 2>/dev/null | head -1`

if [ -z $EXE ]
then
PRG=`ls *.prg 2>/dev/null | head -1`
echo "Compiling PRG $PRG"
cd -
./system/div-LINUX -c zipdiv/$PRG 
cp -r system/EXEC.EXE zipdiv/EXEC.EXE
cd - > /dev/null
EXE=EXEC.EXE
fi

echo "Found EXE: [$EXE]" 
cp $EXE /tmp/EXEC.EXE
cd - >/dev/null
mv /tmp/EXEC.EXE .

THREE="$3"
FOUR="$4"

if [ -z "$3" ]; then
THREE="$2"
fi

if [ -z "$4" ]; then
FOUR="$2"
fi

if [[ $HTMLBUILD -gt 0 ]]
then
echo "Creating html"
./tools/makehtml.bat ./zipdiv $EXE "$2" "$THREE" "$FOUR" 
#> /dev/null
#rm -rf buildhtml buildhtml2
fi

if [[ $GCWBUILD -gt 0 ]]
then
echo "Creating gcw opk"
./tools/makegcw.bat ./zipdiv "$EXE" "$2" "$THREE" "$FOUR"
#scp $2.opk root@192.168.0.12:/media/data/apps
rm -rf buildgcw
fi


if [[ $DROIDBUILD -gt 0 ]]
then
echo "Creating Android apk"
./tools/makedroid.bat ./zipdiv "$EXE" "$2" "$THREE" "$FOUR"
fi

if [[ $PANDORABUILD -gt 0 ]]
then
echo "Creating Pandora PND"
./tools/makepnd.bat ./zipdiv "$EXE" "$2" "$THREE" "$FOUR"
fi


# build packer

gcc ./src/other/pack.c -o pack

#mv zipdiv/EXEC.EXE EXEC.EXE
rm zipdiv/*.prg 2> /dev/null
rm zipdiv/*.PRG 2> /dev/null

rm data.div 2> /dev/null

zip -9jr data.div.zip zipdiv > /dev/null
zip -d data.div.zip "*.pak" > /dev/null
zip -d data.div.zip "*.exe" > /dev/null
zip -d data.div.zip "*.fl*" > /dev/null
mv data.div.zip data.div

VER=`dd if=EXEC.EXE bs=1 count=1 skip=2 2>/dev/null`

if [ $VER = "s" ]
then
RUNTIME=div1run
else
RUNTIME=divrun
fi

if [[ $PIBUILD -gt 0 ]]
then
#### pi build
PATH=$PATH:/home/mike/raspidev/tools/arm-bcm2708/arm-bcm2708hardfp-linux-gnueabi/bin/
cmake . -DTARGETOS=PI
make -j5 $RUNTIME-PI
strip ./system/$RUNTIME-PI
upx -9 ./system/$RUNTIME-PI

./pack ./system/$RUNTIME-PI EXEC.EXE data.div $2-BIN
chmod +x $2-BIN
mkdir $2-PI
mv $2-BIN $2-PI/$2-PI
cp zipdiv/README.md $2-PI
cp zipdiv/LICENSE $2-PI
cp zipdiv/*.pak $2-PI
rm $2-PI.tar.gz
tar zcf $2-PI.tar.gz $2-PI
rm -rf $2-PI
fi


if [[ $WINDOWSBUILD -gt 0 ]]
then
#### winrum build
rm -rf CMakeFiles CMakeCache.txt
cmake . -DTARGETOS=WINDOWS -DCPUARCH=32
make -j5 $RUNTIME-WINDOWS
upx -9 ./system/$RUNTIME-WINDOWS.exe

./pack ./system/$RUNTIME-WINDOWS.exe EXEC.EXE data.div $2.exe

rm $2-win32.zip 2> /dev/null
zip -j9 $2-win32.zip $2.exe system/lib*.dll system/SDL*.dll system/smpeg.dll zipdiv/README.md zipdiv/LICENSE zipdiv/*.pak > /dev/null 
zip -d $2-win32.zip libstdc*.dll

rm $2.exe 2> /dev/null
fi


if [[ $PSPBUILD -gt 0 ]]
then
## psp build
EBOOTDIR=$2-PSP
rm -rf $EBOOTDIR
mkdir $EBOOTDIR
PSPDEV=/home/mike/pspdev
PATH=$PATH:$PSPDEV/bin:$PSPDEV/psp/bin
cmake . -DTARGETOS=PSP
make $RUNTIME-PSP
psp-fixup-imports system/$RUNTIME-PSP 
#-o $EBOOTDIV/divrun-PSP.elf
#cp system/$RUNTIME-PSP $EBOOTDIR/divrun-PSP.elf
psp-strip system/$RUNTIME-PSP -o $EBOOTDIR/$RUNTIME-PSP.elf
mksfo "$THREE" $EBOOTDIR/PARAM.SFO

#psp-prxgen $EBOOTDIR/divrun-PSP.elf $EBOOTDIR/divrun-PSP.prx
#pack-pbp $EBOOTDIR/EBOOT.PBP_ $EBOOTDIR/PARAM.SFO NULL NULL NULL NULL NULL $EBOOTDIR/divrun-PSP.prx NULL 
pack-pbp $EBOOTDIR/EBOOT.PBP_ $EBOOTDIR/PARAM.SFO NULL NULL NULL NULL NULL $EBOOTDIR/$RUNTIME-PSP.elf NULL 

./pack ./$EBOOTDIR/EBOOT.PBP_ EXEC.EXE data.div $EBOOTDIR/EBOOT.PBP 

#ebootsign ./$EBOOTDIR/EBOOT.PBP ./$EBOOTDIR/EBOOT_SIGNED.PBP


rm ./$EBOOTDIR/EBOOT.PBP_ ./$EBOOTDIR/$RUNTIME-PSP.*
cp zipdiv/README.md zipdiv/LICENSE/ $EBOOTDIR
zip -9r $2-PSP.zip $EBOOTDIR
fi


if [[ $LINUXBUILD -gt 0 ]]
then
#### linux build
cmake . -DTARGETOS=LINUX -DCPUARCH=64
make -j5 $RUNTIME-LINUX
upx -9 ./system/$RUNTIME-LINUX
./pack ./system/$RUNTIME-LINUX EXEC.EXE data.div $2-BIN
chmod +x $2-BIN
mkdir $2-LINUX
mv $2-BIN $2-LINUX/$2-LINUX
cp zipdiv/README.md $2-LINUX
cp zipdiv/LICENSE $2-LINUX
cp zipdiv/*.pak $2-LINUX

rm $2-LINUX.tar.gz
tar zcf $2-LINUX.tar.gz $2-LINUX
rm -rf $2-LINUX
fi


# upload files

scp $2-win32.zip $2-*.tar.gz $2.* $2-PSP.zip android/$2.* html/$2.* js.mikedx.co.uk:/var/www/mikedx/js

#delete temp files

rm -rf $2-PSP.zip $EBOOTDIR buildhtml pack buildgcw zipdiv data.div EXEC.EXE $2-PI.tar.gz $2-LINUX.tar.gz $2-win32.zip $2.opk $2.pnd android/$2.apk


#emrun html/$2.html
