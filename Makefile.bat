REM set gcc=E:\Sourcery_CodeBench_Lite_for_ARM_GNU_Linux\bin\arm-none-linux-gnueabi-gcc.exe
set gcc=arm-none-linux-gnueabi-gcc.exe
set ar=arm-none-linux-gnueabi-ar.exe
set hiredisdir=hiredis
set mxmldir=mxml-release-2.10
set mxmllibs=./outputwin/mxml-index.o  ./outputwin/mxml-search.o ./outputwin/mxml-attr.o ./outputwin/mxml-private.o ./outputwin/mxml-entity.o ./outputwin/mxml-get.o ./outputwin/mxml-file.o ./outputwin/mxml-node.o ./outputwin/mxml-set.o ./outputwin/mxml-string.o 
set hiredislibs=./outputwin/dict.o ./outputwin/net.o ./outputwin/read.o ./outputwin/sds.o ./outputwin/hiredis.o
	REM %gcc% -c %mxmldir%/mxml-index.c -I%mxmldir%/ -o ./outputwin/mxml-index.o
	REM %gcc% -c %mxmldir%/mxml-search.c -I%mxmldir%/ -o ./outputwin/mxml-search.o
	REM %gcc% -c %mxmldir%/mxml-attr.c -I%mxmldir%/ -o ./outputwin/mxml-attr.o
	REM %gcc% -c %mxmldir%/mxml-private.c -I%mxmldir%/ -o ./outputwin/mxml-private.o
	REM %gcc% -c %mxmldir%/mxml-entity.c -I%mxmldir%/ -o ./outputwin/mxml-entity.o
	REM %gcc% -c %mxmldir%/mxml-get.c -I%mxmldir%/ -o ./outputwin/mxml-get.o
	REM %gcc% -c %mxmldir%/mxml-file.c -I%mxmldir%/ -o ./outputwin/mxml-file.o
	REM %gcc% -c %mxmldir%/mxml-node.c -I%mxmldir%/ -o ./outputwin/mxml-node.o
	REM %gcc% -c %mxmldir%/mxml-set.c -I%mxmldir%/ -o ./outputwin/mxml-set.o
	REM %gcc% -c %mxmldir%/mxml-string.c -I%mxmldir%/ -o ./outputwin/mxml-string.o

    REM %gcc% -c %hiredisdir%/dict.c -I%hiredisdir%/ -o ./outputwin/dict.o
	REM %gcc% -c %hiredisdir%/net.c -I%hiredisdir%/ -o ./outputwin/net.o
	REM %gcc% -c %hiredisdir%/read.c -I%hiredisdir%/ -o ./outputwin/read.o
	REM %gcc% -c %hiredisdir%/sds.c -I%hiredisdir%/ -o ./outputwin/sds.o
	REM %gcc% -c %hiredisdir%/hiredis.c -I%hiredisdir%/ -o ./outputwin/hiredis.o
	REM %ar% -r winmxmllibs.a  %mxmllibs%
	REM %ar% -r winhiredis.a %hiredislibs%
%gcc% server.c   winmxmllibs.a  winhiredis.a  -o modbus-tcp-server -I./mxml-release-2.10  -I./hiredis


