
# Libpawdfox

A library for firefox password viewers.

# Build Instructions

libpawdfox requires rapidjson, libnss3, libnspr4 and autotools.

### Mac OS X
Install XCode and Homebrew

    brew install nss nspr autoconf automake rapidjson

### Ubuntu

    apt install rapidjson-dev libnss3-dev libnss3 libnspr4 libnspr4-dev autoconf autotools-dev build-essential

## autotools

Use autotools to generate support files to build it.

Run the following commands to generate configure script.

    # aclocal
    # glibtoolize
    # automake --add-missing
    # autoconf

You can run ./configure only if the dependencies are all on system path. Or you can set specific path as follow:

    LDFLAGS=-L/usr/local/opt/nss/lib\ -L/usr/local/opt/nspr/lib/ CPPFLAGS=-I/usr/local/opt/nspr/include/nspr/\ -I/usr/local/opt/nss/include/ ./configure

You can also add -DDEBUG to enable debug messages.


## Windows

To build on Windows is necessary to build NSS3.

https://developer.mozilla.org/en-US/docs/Mozilla/Developer_guide/Build_Instructions/Windows_Prerequisites

Install mozillabuild

### Using Visual Studio Prompt
Start DEV shell

    start-shell
    cmd /k /c/Program\ Files\ \(x86\)/Microsoft\ Visual\ Studio/2017/Community/Common7/Tools/vsdevcmd.bat   

Build NSS

    set NSS_DISABLE_GTESTS=1
    make nss_build_all

Build libpawdfox

    cl /c /EHsc /I%RAPIDJSONPATH%\rapidjson\include  /I%NSSPATH%\nss-3.34.1\dist\WIN954.0_DBG.OBJ\include /I%NSSPATH%\nss-3.34.1\dist\public\ libpawdfox.cpp

### Using MINGW

    g++ -c -fno-keep-inline-dllexport -Os -momit-leaf-frame-pointer -O2 -std=gnu++11 -Wextra -Wall -W -fexceptions -mthreads -I${RAPIDJSONPATH}\rapidjson\include -I${NSSPATH}\nss-3.34.1\dist\WIN954.0_DBG.OBJ\include -I${NSSPATH}\nss-3.34.1\dist\public -o libpawdfox.o -lnss3 libpawdfox.cpp

PS: Order matter, -lnss3 must go after libpawdfox.o

## Build Example

You can try an example using main.cpp file on src/ dir.

### Windows

Build main.cpp

    cl /EHsc /I%RAPIDJSONPATH%\rapidjson\include  /I%NSSPATH%\nss-3.34.1\dist\WIN954.0_DBG.OBJ\include /I%NSSPATH%\nss-3.34.1\dist\public\  main.cpp libpawdfox.lib nss3.lib /link /LIBPATH:%NSSPATH%\nss-3.34.1\dist\WIN954.0_DBG.OBJ\lib\

### Linux and Mac OS X

    g++ -std=c++11 -L/usr/local/opt/nss/lib -lnss3 main.cpp libpawdfox.o -o main
