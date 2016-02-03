#!/bin/bash

rm -rf zipdiv
mkdir zipdiv
cd zipdiv
echo "unzipping file"

unzip -j $1 > /dev/null

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
cd - > /dev/null
EXE=EXEC.EXE
fi

echo "Found EXE: [$EXE]" 
cd - >/dev/null
#pwd

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

mv zipdiv/EXEC.EXE .
rm zipdiv/*.prg
rm zipdiv/*.PRG

rm data.div

zip -r -j data.div zipdiv

#### linux build
./pack ./system/divrun-LINUX EXEC.EXE data.div $2-BIN
chmod +x $2-BIN
mkdir $2-LINUX
mv $2-BIN $2-LINUX/$2-LINUX
cp $2/README.md $2-LINUX
cp $2/LICENSE $2-LINUX
rm $2-LINUX.tar.gz
tar zcf $2-LINUX.tar.gz $2-LINUX
rm -rf $2-LINUX

#### pi build

./pack ./system/divrun-PI EXEC.EXE data.div $2-BIN
chmod +x $2-BIN
mkdir $2-PI
mv $2-BIN $2-PI/$2-PI
cp $2/README.md $2-PI
cp $2/LICENSE $2-PI
rm $2-PI.tar.gz
tar zcf $2-PI.tar.gz $2-PI
rm -rf $2-PI


#### winrum build

./pack ./system/divrun-WINDOWS.exe EXEC.EXE data.div $2.exe

rm $2-win64.zip
zip -j $2-win64.zip $2.exe system/lib*.dll system/SDL*.dll $2/README.md $2/LICENSE > /dev/null
rm $2.exe

# upload files

scp $2-win64.zip $2-*.tar.gz $2.* android/$2.* html/$2.* js.mikedx.co.uk:/var/www/mikedx/js

rm -rf zipdiv data.div EXEC.EXE $2-PI.tar.gz $2-LINUX.tar.gz $2-win64.zip $2.opk $2.pnd android/$2.apk


#emrun html/$2.html
