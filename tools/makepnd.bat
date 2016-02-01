#!/bin/bash

TARGET='/tmp/buildpandora'
OLDPWD=`pwd`

#echo "1:$1 2:$2 3:$3 4:$4"

echo "Creating build dir"

rm -rf $TARGET
mkdir -p "$TARGET"


echo "Copying source files"
cp -r $1/* $TARGET

#echo "renaming files to owercase"

#find . -depth -exec rename 's/(.*)\/([^\/]*)/$1\/\L$2/' {} \;

echo "Removing old div exe"
rm "$TARGET/$2"

echo "Removing extra exe files"
find "$TARGET" -iname "*.exe" | xargs rm 2>/dev/null

echo "Removing dll files"
find "$TARGET" -iname "div*run.dll" | xargs rm 2>/dev/null

echo "Removing flic files"
find "$TARGET" -iname "*.fl*" | xargs rm 2>/dev/null


PAK=`find "$TARGET" -iname '*.pak' | wc -l`

echo "PAK FOUND: $PAK"

if [ $PAK == "0" ]
then

echo "Compressing resources to data.div file"

cd "$TARGET"
zip ../data.div -9 -r *
rm -rf *
mv ../data.div .

cd -
fi


mkdir -p "$TARGET/previews"
mkdir -p "$TARGET/scripts"

echo "Copying new exe files"


VER=`dd if="$1/$2" bs=1 count=1 skip=2 2>/dev/null`
#echo $VER

cmake . -DTARGETOS=PANDORA
export PATH=/home/mike/pandora-dev/arm-2011.09/bin/:$PATH

# s= div1 j=div2
if [ $VER = "s" ]
then
echo "DIV1 runtime"
make div1run-PANDORA > /dev/null
/home/mike/pandora-dev/arm-2011.09/bin/pandora-strip system/*-PANDORA

cp div1run-PANDORA $TARGET/$3
#cp android/div1.so android/div2-droid/lib/armeabi/libapplication.so
#cp "$1/$2" "android/resources/run"

else
echo "DIV2 runtime"
make divrun-PANDORA > /dev/null
/home/mike/pandora-dev/arm-2011.09/bin/pandora-strip system/*-PANDORA

cp system/divrun-PANDORA $TARGET/$3
#cp android/div2.so android/div2-droid/lib/armeabi/libapplication.so
#cp "$1/$2" "android/resources/EXEC.EXE"

fi


cp "$1/$2" "$TARGET/$3.dat"
cp tools/dx.png "$TARGET"/icon.png

uid=${5,,}
uid=${uid//[[:blank:]]/}

cd "$TARGET"

if [ $VER = "s" ]
then
/home/mike/div2015/div1run-LINUX $3.dat

else 

/home/mike/div2015/system/divrun-LINUX $3.dat

fi



convert *.PCX previews/$3.png
rm *.PCX

PREVIEWS=""
for i in previews/*.png
do

PREVIEWS=$PREVIEWS"<pic src=\""$i"\"/>"

done

cd -

echo "Creating command script file $3.sh"
cat << EOF > "$TARGET/scripts/$3.sh"
#!/bin/bash
export SDL_VIDEODRIVER=omapdss
export SDL_OMAP_LAYER_SIZE=scaled
./$3 $3.dat
EOF

chmod +x "$TARGET/scripts/$3.sh"


echo "Creating PND XML CONFIG FILE"
cat << EOF > "$TARGET/PXML.xml"
<?xml version="1.0" encoding="UTF-8"?>
<PXML xmlns="http://openpandora.org/namespaces/PXML">
<!-- please see http://pandorawiki.org/PXML_specification for more information before editing, and remember the order does matter -->
  <application id="div2015-$3-$uid" appdata="div2015-$3">
    <exec command="scripts/$3.sh"/>
    <title lang="en_US">$4</title>

    <author name="$5" website="http://div-arena.co.uk/"/>
    <version major="0" minor="0" release="1" build="1"/>	      <!--This programs version-->
    <osversion major="1" minor="0" release="0" build="0"/>		<!--The minimum OS version required-->

    <description lang="en_US">Made with DIV Games studio. <br />Play this game online at http://js.mikedx.co.uk/$3.html</description>

    <icon src="icon.png"/>

    <previewpics>
	$PREVIEWS
    </previewpics>

    <!--<info name="kartdemo documentation" type="text/plain" src="/home/mike/div2015/kartpnd//readme.txt"/>-->

    <categories>
      <!-- http://standards.freedesktop.org/menu-spec/latest/apa.html -->
      <category name="Game">
	<subcategory name="ArcadeGame"/>
      </category>
    </categories>

<!--
    <associations>
      <association name="Deinterlaced Bitmap Image" filetype="image/bmp" exec="-f %s"/>
      <association name="Style sheet system crasher" filetype="text/css" exec="-f %s"/>
    </associations>
-->
    <!--<clockspeed frequency="600"/>-->
  </application>

</PXML>

EOF
#echo "Making resources file"

pwd

/home/mike/pandora-dev/sdk_utils/pnd_make.sh -d "$TARGET" -c  -i "$TARGET"/icon.png  -p $3.pnd 

echo "Done! - created $3.pnd"

