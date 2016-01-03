LOCAL_PATH=`dirname $0`
LOCAL_PATH=`cd $LOCAL_PATH && pwd`



#ln -sf libsdl-1.2.so $LOCAL_PATH/../../../obj/local/$1/libSDL.so

#if [ \! -f newvox-1.0/configure ] ; then
#	sh -c "cd newvox-1.0 && ./autogen.sh"
#fi
#
#if [ \! -f newvox-1.0/Makefile ] ; then
rm -rf CMakeCache.txt CMakeFiles cmake_install.cmake 
../setEnvironment-$1.sh sh -c "cmake . -DTARGETOS=ANDROID && make divrun-LINUX VERBOSE=1"
cp system/divrun-LINUX libapplication-$1.so
rm system/divrun-LINUX

#rm -rf CMakeCache CMakeFiles cmake_install.cmake 
#../setEnvironment-armeabi-v7a-hard.sh sh -c "cmake . -DTARGETOS=ANDROID && make divrun-LINUX"
#cp system/divrun-LINUX libapplication-armeabi-v7a-hard.so

#rm -rf CMakeCache CMakeFiles cmake_install.cmake 
#../setEnvironment-mips.sh sh -c "cmake . -DTARGETOS=ANDROID && make divrun-LINUX"
#cp system/divrun-LINUX libapplication-mips.so


#rm -rf CMakeCache CMakeFiles cmake_install.cmake 
#../setEnvironment-x86.sh sh -c "cmake . -DTARGETOS=ANDROID && make divrun-LINUX"
#cp system/divrun-LINUX libapplication-x86.so


# sh -c "cd newvox-1.0 && ./configure --host=arm-linux-androideabi"
#fi

#cmake .
#make
# -C newvox-1.0 && mv -f newvox-1.0/newvox libapplication.so

