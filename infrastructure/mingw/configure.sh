#!/bin/sh

source `dirname $0`/environment.sh

if [ ! -r "$DEP_PATH/lib/libssl.a" ]; then
	echo "Error: install OpenSSL as instructed by" \
		"docs/backup/win32_build_on_cygwin_using_mingw.txt" >&2
	exit 2
fi

if [ ! -r "$DEP_PATH/lib/libpcreposix.a" \
	-o ! -r "$DEP_PATH/lib/libpcre.a" \
	-o ! -r "$DEP_PATH/include/pcreposix.h" ]; then
	echo "Error: install PCRE as instructed by" \
		"docs/backup/win32_build_on_cygwin_using_mingw.txt" >&2
	exit 2
fi


if [ ! -r "$DEP_PATH/lib/libz.dll.a" ]; then
	echo "Error: upgrade your Cygwin mingw-zlib-devel package" >&2
	exit 2
fi

echo "Running bootstrap"
if [ ! -x "configure" ]; then
	if ! ./bootstrap; then
		echo "Error: bootstrap failed, aborting." >&2
		exit 1
	fi
fi

echo "Running configure"
if ! ./configure -v --build=$host --host=$host "$@" \
	CPPFLAGS="-I${DEP_PATH}/include " \
	CFLAGS="-mthreads" \
	CXXFLAGS="-mthreads" \
	LDFLAGS="-Wl,-Bstatic -mthreads -L${DEP_PATH}/lib" \
# msvcrt needs a manifest :(
#	LIBS="-lmsvcr80"
then
	echo "Error: configure failed, aborting." >&2
	cat config.log
	exit 1
fi

exit 0
