#!/bin/bash

rm -rf zipdiv
mkdir zipdiv
cd zipdiv
echo "unzipping file"

unzip -j $1 2>/dev/null

echo "converting files to lowercase"

find . -depth -exec rename 's/(.*)\/([^\/]*)/$1\/\L$2/' {} \;

rm setup.exe 

EXE=`ls *.exe 2>/dev/null | head -1`

if [ -z $EXE ]
then
PRG=`ls *.prg 2>/dev/null | head -1`
echo "Compiling PRG $PRG"
cd -
./system/div-LINUX -c zipdiv/$PRG 
cp -r system/EXEC.EXE zipdiv/EXEC.EXE
cd - 2> /dev/null
EXE=EXEC.EXE
fi


echo "Found EXE: [$EXE]" 
pwd
cp $EXE /tmp/EXEC.EXE
cd - >/dev/null
mv /tmp/EXEC.EXE .
pwd
#exit

THREE="$3"
FOUR="$4"

if [ -z "$3" ]; then
THREE="$2"
fi

if [ -z "$4" ]; then
FOUR="$2"
fi

echo "Creating html"
./tools/makehtml.bat ./zipdiv $EXE "$2" "$THREE" "$FOUR"

echo "Creating gcw opk"
./tools/makegcw.bat ./zipdiv "$EXE" "$2" "$THREE" "$FOUR"
#scp $2.opk root@192.168.0.12:/media/data/apps

echo "Creating Android apk"
./tools/makedroid.bat ./zipdiv "$EXE" "$2" "$THREE" "$FOUR"

echo "Creating Pandora PND"
./tools/makepnd.bat ./zipdiv "$EXE" "$2" "$THREE" "$FOUR"


# build packer

gcc ./src/other/pack.c -o pack

#mv zipdiv/EXEC.EXE EXEC.EXE
rm zipdiv/*.prg 2> /dev/null
rm zipdiv/*.PRG 2> /dev/null

rm data.div 2> /dev/null

zip -r -j data.div zipdiv 2> /dev/null
zip -d data.div "*.pak"
zip -d data.div "*.exe"


VER=`dd if=EXEC.EXE bs=1 count=1 skip=2 2>/dev/null`

if [ $VER = "s" ]
then
RUNTIME=div1run
else
RUNTIME=divrun
fi

#### linux build
cmake . -DTARGETOS=LINUX
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


#### winrum build
cmake . -DTARGETOS=WINDOWS
make -j5 $RUNTIME-WINDOWS
upx -9 ./system/$RUNTIME-WINDOWS.exe

./pack ./system/$RUNTIME-WINDOWS.exe EXEC.EXE data.div $2.exe

rm $2-win64.zip 2> /dev/null
zip -j $2-win64.zip $2.exe system/lib*.dll system/SDL*.dll zipdiv/README.md zipdiv/LICENSE zipdiv/*.pak > /dev/null 
rm $2.exe 2> /dev/null

# upload files

scp $2-win64.zip $2-*.tar.gz $2.* android/$2.* html/$2.* js.mikedx.co.uk:/var/www/mikedx/js

rm -rf buildhtml pack buildgcw zipdiv data.div EXEC.EXE $2-PI.tar.gz $2-LINUX.tar.gz $2-win64.zip $2.opk $2.pnd android/$2.apk


#emrun html/$2.html
