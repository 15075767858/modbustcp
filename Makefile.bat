REM set gcc=E:\Sourcery_CodeBench_Lite_for_ARM_GNU_Linux\bin\arm-none-linux-gnueabi-gcc.exe
set gcc=arm-none-linux-gnueabi-gcc.exe
set ar=arm-none-linux-gnueabi-ar.exe
set hiredisdir=hiredis
set mxmldir=mxml-release-2.10
set mxmllibs=./outputwin/mxml-index.o  ./outputwin/mxml-search.o ./outputwin/mxml-attr.o ./outputwin/mxml-private.o ./outputwin/mxml-entity.o ./outputwin/mxml-get.o ./outputwin/mxml-file.o ./outputwin/mxml-node.o ./outputwin/mxml-set.o ./outputwin/mxml-string.o 
set hiredislibs=./outputwin/dict.o ./outputwin/net.o ./outputwin/read.o ./outputwin/sds.o ./outputwin/hiredis.o
	%gcc% -c %mxmldir%/mxml-index.c -I%mxmldir%/ -o ./outputwin/mxml-index.o
	%gcc% -c %mxmldir%/mxml-search.c -I%mxmldir%/ -o ./outputwin/mxml-search.o
	%gcc% -c %mxmldir%/mxml-attr.c -I%mxmldir%/ -o ./outputwin/mxml-attr.o
	%gcc% -c %mxmldir%/mxml-private.c -I%mxmldir%/ -o ./outputwin/mxml-private.o
	%gcc% -c %mxmldir%/mxml-entity.c -I%mxmldir%/ -o ./outputwin/mxml-entity.o
	%gcc% -c %mxmldir%/mxml-get.c -I%mxmldir%/ -o ./outputwin/mxml-get.o
	%gcc% -c %mxmldir%/mxml-file.c -I%mxmldir%/ -o ./outputwin/mxml-file.o
	%gcc% -c %mxmldir%/mxml-node.c -I%mxmldir%/ -o ./outputwin/mxml-node.o
	%gcc% -c %mxmldir%/mxml-set.c -I%mxmldir%/ -o ./outputwin/mxml-set.o
	%gcc% -c %mxmldir%/mxml-string.c -I%mxmldir%/ -o ./outputwin/mxml-string.o

    %gcc% -c %hiredisdir%/dict.c -I%hiredisdir%/ -o ./outputwin/dict.o
	%gcc% -c %hiredisdir%/net.c -I%hiredisdir%/ -o ./outputwin/net.o
	%gcc% -c %hiredisdir%/read.c -I%hiredisdir%/ -o ./outputwin/read.o
	%gcc% -c %hiredisdir%/sds.c -I%hiredisdir%/ -o ./outputwin/sds.o
	%gcc% -c %hiredisdir%/hiredis.c -I%hiredisdir%/ -o ./outputwin/hiredis.o
	%ar% -r winmxmllibs.a  %mxmllibs%
	%ar% -r winhiredis.a %hiredislibs%
%gcc% server.c   winmxmllibs.a  winhiredis.a  -o modbus-tcp-server -I./mxml-release-2.10  -I./hiredis


