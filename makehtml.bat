#!/bin/bash

echo "Setting up emscripten"

source ~/emsdk_portable/emsdk_env.sh > /dev/null

rm -rf obj obj_d objd1

#dd if="$1/$2" bs=1 count=1 skip=2
VER=`dd if="$1/$2" bs=1 count=1 skip=2 2>/dev/null`
#echo "VER = '$VER'"

echo "Building $4"

echo "Compressing dir $1"

rm -rf buildhtml
mkdir buildhtml
cp -R "$1/"* buildhtml
rm -rf buildhtml2
mkdir buildhtml2

if [ $1 != "1" ] 
then

rm buildhtml/$2

#rm data.div
cd buildhtml

find . -depth -exec rename 's/(.*)\/([^\/]*)/$1\/\L$2/' {} \;

rm *.exe
mv *.pak ../buildhtml2
rm *.prg
rm *.dll

echo "Creating data.div"
zip ../buildhtml2/data.div -r -9 * > /dev/null
cd -

cp "$1/$2" buildhtml2

else
rm -rf buildhtml2
mv buildhtml buildhtml2

fi

# s= div1 j=div2
if [ $VER = "s" ] 
then
echo "USING DIV1 RUNTIME"
make -f Makefile.html dirs R1EMBED="--preload-file ./buildhtml2@" D1HTML_EXE="$2" DIV1RUN="html/$3.js" html/$3.js > /dev/null
DIVVER="DIV1"
else
echo "USING DIV2 RUNTIME"	
make -f Makefile.html dirs DEMBEDFILES="--preload-file ./buildhtml2@" HTML_EXE="$2" RUNNER="html/$3.js" html/$3.js > /dev/null
DIVVER="DIV2"
fi


#make -f Makefile.html dirs R1EMBED="--preload-file $1@" D1HTML_EXE="$2" DIV1RUN="html/$3.js" html/$3.js > /dev/null

rm -rf obj obj_d objd1

echo "Creating html file"

cat << EOF > html/$3.html
<!DOCTYPE html>
<html lang="en">
	<head>
		<meta content="text/html; charset=utf-8" http-equiv="Content-Type">
		<meta content=
		"width=device-width, initial-scale=1.0, maximum-scale=1.0, minimum-scale=1.0"
		name="viewport">
		<script src="site/jquery.min.js"></script>
		<meta charset="utf-8">
		<meta content="text/html; charset=utf-8" http-equiv="Content-Type">
		<title>
			DIV GAMES STUDIO - $4
		</title>
		<link href="site/all.css" rel="stylesheet">
		<link href="themes/cyborg.min.css" rel="stylesheet">
		<link href="themes/bootstrap-responsive.min.css" rel="stylesheet">
	</head>
	<body>
		<script async="" src="site/load.js" type='text/javascript'></script>
		<div class="navbar navbar-fixed-top">
			<div class="navbar-inner">
				<div class="container">
					<div class="brand" id="headline">
						DIV GAMES STUDIO - $4
					</div>
					<div class="brand" id="fps">
						FPS: 0
					</div>
				</div>
			</div>
		</div>
		<div class="container" id="content">
			<h2>
						DIV GAMES STUDIO - $4 
			</h2>
			
			<p>$DIVVER Engine ported to C / SDL, compiled to
				Javascript using Emscripten</p>
				<p>By $5</p>
				<p>$controls<br />
					<span id='controls'>
               <span style="display:none;">
					<input type="hidden" id="resize">Resize canvas
				</span>
               <span style="display:none;">
					<input type="checkbox" id="pointerLock">
					Lock/hide mouse pointer &nbsp;&nbsp;&nbsp;
				</span>
			</span>
</p>
			<div class="spinner" id='spinner'></div>
		<div class="emscripten" id="status">
			Downloading...
		</div>
		<div class="emscripten">
			<progress hidden="1" id="progress" max="100" value="0"></progress>
		</div>
		</div>
		<div class='container'>
			<canvas class="emscripten" id="canvas" oncontextmenu=
				"event.preventDefault()"></canvas>
			<input id='fullscreenbtn' class="btn btn-primary" type="button" value="Play in Fullscreen" />
			<textarea class='hidden-phone hidden-tablet hidden-desktop' id="output" rows="8">
			</textarea>
		</div>
		
		<script async="" src="$3.js" type="text/javascript"></script>
		<script async="" src="site/analytics.js" type="text/javascript"></script>
		
	</body>
	
</html>


EOF

echo "Files copied to html/$3.*"
echo "Use emrun html/$3.html to run / test"

echo "Done!"


 
