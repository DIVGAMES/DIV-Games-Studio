#!/bin/bash

FIVE="$5"
FOUR="$4"

OPATH="/var/www/div-arenav4/divarena/webroot/downloads/"

if [[ -n $6 ]]; then
	OUT="$6"
	echo "SIX IS $6"
fi


if [ -z $FIVE ]; then
	FIVE="none"
fi

echo "FIVE IS $FIVE"
echo $5

echo "FOUR IS $FOUR"
echo $4

rm -rf zipdiv
mkdir zipdiv
cd zipdiv
echo "unzipping file"

unzip -j $1
#/$6/../dos/$7.zip
#2>/dev/null

echo "converting files to lowercase"

find . -depth -exec rename 's/(.*)\/([^\/]*)/$1\/\L$2/' {} \;

rm setup.exe 

EXE=`ls *.exe 2>/dev/null | head -1`

if [ -z $EXE ]; then
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

if [ -n "$3" ]; then
	THREE="$2"
fi

if [ -n "$4" ]; then
	FOUR="$2"
fi

echo "FIVE IS $FIVE"
if [[ $FIVE == "html" ]]; then
	echo "Creating $2 $THREE FOUR html"
	./tools/makehtml.bat ./zipdiv $EXE "$2" "$THREE" "$FOUR"

	echo "copying to $OUT/$7.js"
	
	if [[ ! -z "$OUT" ]]; then
		mkdir -p $OPATH/$OUT
		cp html/$2.js $OPATH/$OUT/$7.js
		cp html/$2.* $OPATH/$OUT/
	fi
	exit
fi

if [ "$FIVE" == "gcw" ]; then
	echo "Creating gcw opk"
	./tools/makegcw.bat ./zipdiv "$EXE" "$2" "$THREE" "$FOUR"
	rm -rf buildgcw
fi

if [ "$FIVE" == "android" ]; then
	echo "Creating Android apk"
	./tools/makedroid.bat ./zipdiv "$EXE" "$2" "$THREE" "$FOUR"
fi

if [ "$FIVE" == "padora" ]; then
	echo "Creating Pandora PND"
	./tools/makepnd.bat ./zipdiv "$EXE" "$2" "$THREE" "$FOUR"
fi

# build packer

if [ ! -f ./pack ]; then
	gcc ./src/other/pack.c -o pack

	#mv zipdiv/EXEC.EXE EXEC.EXE
	rm zipdiv/*.prg 2> /dev/null
	rm zipdiv/*.PRG 2> /dev/null

	rm data.div 2> /dev/null
fi

zip -9jr data.div.zip zipdiv 2> /dev/null
zip -d data.div.zip "*.pak"
zip -d data.div.zip "*.exe"
zip -d data.div.zip "*.fl*"
mv data.div.zip data.div

VER=`dd if=EXEC.EXE bs=1 count=1 skip=2 2>/dev/null`

if [ $VER = "s" ]; then
	RUNTIME=div1run
else
	RUNTIME=divrun
fi

#### pi build
if [ "$FIVE" == "pi" ]; then
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

#### winrum build
if [ "$FIVE" == "win" ]; then
	cmake . -DTARGETOS=WINDOWS
	make -j5 $RUNTIME-WINDOWS
	upx -9 ./system/$RUNTIME-WINDOWS.exe

	./pack ./system/$RUNTIME-WINDOWS.exe EXEC.EXE data.div $2.exe

	rm $2-win32.zip 2> /dev/null
	zip -j9 $2-win32.zip $2.exe windlls/lib*.dll windlls/SDL*.dll zipdiv/README.md zipdiv/LICENSE zipdiv/*.pak > /dev/null 
	zip -d $2-win32.zip libstdc*.dll

	rm $2.exe 2> /dev/null
fi

## psp build
if [ "$FIVE" == "psp" ]; then
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

echo "FIVE IS $FIVE"

#### linux build
if [ "m" == "linux" ]; then
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

#	if [ ! -z $OUT ]; then
#		mv $2-LINUX.tar.gz $OPATH/$OUT
#	fi

fi

