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

echo "Creating Android apk"
./tools/makedroid.bat ./zipdiv "$EXE" "$2" "$THREE" "$FOUR"

#rm -rf zipdiv


scp $2.opk root@192.168.0.12:/media/data/apps
emrun html/$2.html
