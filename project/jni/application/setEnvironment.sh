#!/bin/sh

IFS='
'

MYARCH=linux-x86_64
if uname -s | grep -i "linux" > /dev/null ; then
	MYARCH=linux-x86_64
fi
if uname -s | grep -i "darwin" > /dev/null ; then
	MYARCH=darwin-x86_64
fi
if uname -s | grep -i "windows" > /dev/null ; then
	MYARCH=windows-x86_64
fi

NDK=`which ndk-build`
NDK=`dirname $NDK`
NDK=`readlink -f $NDK`

#echo NDK $NDK
GCCPREFIX=arm-linux-androideabi
[ -z "$NDK_TOOLCHAIN_VERSION" ] && NDK_TOOLCHAIN_VERSION=4.9
PLATFORMVER=android-14
LOCAL_PATH=`dirname $0`
if which realpath > /dev/null ; then
	LOCAL_PATH=`realpath $LOCAL_PATH`
else
	LOCAL_PATH=`cd $LOCAL_PATH && pwd`
fi
ARCH=armeabi

APP_MODULES=`grep 'APP_MODULES [:][=]' $LOCAL_PATH/../Settings.mk | sed 's@.*[=]\(.*\)@\1@'`
APP_AVAILABLE_STATIC_LIBS=`grep 'APP_AVAILABLE_STATIC_LIBS [:][=]' $LOCAL_PATH/../Settings.mk | sed 's@.*[=]\(.*\)@\1@'`
APP_SHARED_LIBS=$(
echo $APP_MODULES | xargs -n 1 echo | while read LIB ; do
	STATIC=`echo $APP_AVAILABLE_STATIC_LIBS application sdl_main stlport stdout-test | grep "\\\\b$LIB\\\\b"`
	if [ -n "$STATIC" ] ; then true
	else
		case $LIB in
			crypto) echo crypto.so.sdl.1;;
			ssl) echo ssl.so.sdl.1;;
			curl) echo curl-sdl;;
			*) echo $LIB;;
		esac
	fi
done
)


MISSING_INCLUDE=
MISSING_LIB=

CFLAGS="\
-fpic -ffunction-sections -funwind-tables -fstack-protector-strong \
-no-canonical-prefixes -march=armv5te -mtune=xscale -msoft-float \
-mthumb -O2 -g -DNDEBUG \
-fomit-frame-pointer -fno-strict-aliasing -finline-limit=300 \
-DANDROID -Wall -Wno-unused -Wa,--noexecstack -Wformat \
-isystem$NDK/platforms/$PLATFORMVER/arch-arm/usr/include \
-isystem$NDK/sources/cxx-stl/gnu-libstdc++/$NDK_TOOLCHAIN_VERSION/include \
-isystem$NDK/sources/cxx-stl/gnu-libstdc++/$NDK_TOOLCHAIN_VERSION/libs/$ARCH/include \
-isystem$NDK/sources/cxx-stl/gnu-libstdc++/$NDK_TOOLCHAIN_VERSION/include/backward \
-isystem$LOCAL_PATH/../sdl-1.2/include \
`echo $APP_MODULES | sed \"s@\([-a-zA-Z0-9_.]\+\)@-isystem$LOCAL_PATH/../\1/include@g\"` \
$MISSING_INCLUDE $CFLAGS"

if [ -z "$SHARED_LIBRARY_NAME" ]; then
	SHARED_LIBRARY_NAME=libapplication.so
fi
UNRESOLVED="-Wl,--no-undefined"
SHARED="-shared -Wl,-soname,$SHARED_LIBRARY_NAME"
if [ -n "$BUILD_EXECUTABLE" ]; then
	SHARED="-Wl,--gc-sections -Wl,-z,nocopyreloc -pie"
fi
if [ -n "$NO_SHARED_LIBS" ]; then
	APP_SHARED_LIBS=
fi
if [ -n "$ALLOW_UNRESOLVED_SYMBOLS" ]; then
	UNRESOLVED=
fi

LDFLAGS="\
$SHARED \
--sysroot=$NDK/platforms/$PLATFORMVER/arch-arm \
-L$LOCAL_PATH/../../obj/local/$ARCH \
`echo $APP_SHARED_LIBS | sed \"s@\([-a-zA-Z0-9_.]\+\)@$LOCAL_PATH/../../obj/local/$ARCH/lib\1.so@g\"` \
-L$NDK/platforms/$PLATFORMVER/arch-arm/usr/lib \
-lc -lm -lGLESv1_CM -ldl -llog -lz \
-L$NDK/sources/cxx-stl/gnu-libstdc++/$NDK_TOOLCHAIN_VERSION/libs/$ARCH \
-lgnustl_static \
-no-canonical-prefixes $UNRESOLVED -Wl,-z,noexecstack -Wl,-z,relro -Wl,-z,now \
-Wl,--build-id -Wl,--warn-shared-textrel -Wl,--fatal-warnings \
-lsupc++ \
$MISSING_LIB $LDFLAGS"

CC="$NDK/toolchains/$GCCPREFIX-$NDK_TOOLCHAIN_VERSION/prebuilt/$MYARCH/bin/$GCCPREFIX-gcc"
CXX="$NDK/toolchains/$GCCPREFIX-$NDK_TOOLCHAIN_VERSION/prebuilt/$MYARCH/bin/$GCCPREFIX-g++"
CPP="$NDK/toolchains/$GCCPREFIX-$NDK_TOOLCHAIN_VERSION/prebuilt/$MYARCH/bin/$GCCPREFIX-cpp $CFLAGS"

if [ -n "$CLANG" ]; then

CFLAGS="\
-gcc-toolchain $NDK/toolchains/$GCCPREFIX-$NDK_TOOLCHAIN_VERSION/prebuilt/$MYARCH \
-target armv5te-none-linux-androideabi -Wno-invalid-command-line-argument -Wno-unused-command-line-argument \
$CFLAGS"

LDFLAGS="$LDFLAGS \
-lgcc \
-latomic \
-gcc-toolchain $NDK/toolchains/$GCCPREFIX-$NDK_TOOLCHAIN_VERSION/prebuilt/$MYARCH \
-target armv5te-none-linux-androideabi"

CC="$NDK/toolchains/llvm/prebuilt/$MYARCH/bin/clang"
CXX="$NDK/toolchains/llvm/prebuilt/$MYARCH/bin/clang++"
CPP="$CC -E $CFLAGS"

fi

env PATH=$NDK/toolchains/$GCCPREFIX-$NDK_TOOLCHAIN_VERSION/prebuilt/$MYARCH/bin:$LOCAL_PATH:$PATH \
CFLAGS="$CFLAGS" \
CXXFLAGS="$CXXFLAGS $CFLAGS -frtti -fexceptions" \
LDFLAGS="$LDFLAGS" \
CC="$CC" \
CXX="$CXX" \
RANLIB="$NDK/toolchains/$GCCPREFIX-$NDK_TOOLCHAIN_VERSION/prebuilt/$MYARCH/bin/$GCCPREFIX-ranlib" \
LD="$CXX" \
AR="$NDK/toolchains/$GCCPREFIX-$NDK_TOOLCHAIN_VERSION/prebuilt/$MYARCH/bin/$GCCPREFIX-ar" \
CPP="$CPP" \
NM="$NDK/toolchains/$GCCPREFIX-$NDK_TOOLCHAIN_VERSION/prebuilt/$MYARCH/bin/$GCCPREFIX-nm" \
AS="$NDK/toolchains/$GCCPREFIX-$NDK_TOOLCHAIN_VERSION/prebuilt/$MYARCH/bin/$GCCPREFIX-as" \
STRIP="$NDK/toolchains/$GCCPREFIX-$NDK_TOOLCHAIN_VERSION/prebuilt/$MYARCH/bin/$GCCPREFIX-strip" \
"$@"
