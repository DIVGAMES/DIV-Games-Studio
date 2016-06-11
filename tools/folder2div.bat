#folder to div
rm /tmp/divtmp.zip
zip -j /tmp/divtmp.zip "$1"/*
./tools/zip2div.bat /tmp/divtmp.zip $2 $3 $4
