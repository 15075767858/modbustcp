REM set gcc=E:\Sourcery_CodeBench_Lite_for_ARM_GNU_Linux\bin\arm-none-linux-gnueabi-gcc.exe
set gcc=arm-none-linux-gnueabi-gcc.exe
set ar=arm-none-linux-gnueabi-ar.exe
set hiredisdir=hiredis
set mxmldir=mxml-release-2.10
set libevetdir=libevent
set libeventlibs = ./outputwin/evutil_rand.o ./outputwin/epoll.o ./outputwin/poll.o ./outputwin/select.o ./outputwin/signal.o ./outputwin/evport.o ./outputwin/devpoll.o ./outputwin/log.o ./outputwin/evthread.o ./outputwin/evutil_time.o ./outputwin/kqueue.o ./outputwin/evmap.o ./outputwin/evutil.o ./outputwin/event.o
set mxmllibs=./outputwin/mxml-index.o  ./outputwin/mxml-search.o ./outputwin/mxml-attr.o ./outputwin/mxml-private.o ./outputwin/mxml-entity.o ./outputwin/mxml-get.o ./outputwin/mxml-file.o ./outputwin/mxml-node.o ./outputwin/mxml-set.o ./outputwin/mxml-string.o 
set hiredislibs=./outputwin/dict.o ./outputwin/net.o ./outputwin/read.o ./outputwin/sds.o ./outputwin/hiredis.o ./outputwin/async.o
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
	REM %gcc% -c %hiredisdir%/async.c -I%hiredisdir%/ -o ./outputwin/async.o
REM %gcc% server.c   winmxmllibs.a  winhiredis.a  -o modbus-tcp-server -I./mxml-release-2.10  -I./hiredis
REM %ar% -r outputwin/winmxmllibs.a  %mxmllibs%	
	REM  %ar% -r outputwin/winhiredis.a %hiredislibs% 

	REM %gcc% -c libevent/evutil_rand.c -Ilibevent/include -Ilibevent/ -o ./outputwin/evutil_rand.o
	REM %gcc% -c libevent/epoll.c -Ilibevent/include -Ilibevent/ -o ./outputwin/epoll.o
	REM %gcc% -c libevent/poll.c -Ilibevent/include -Ilibevent/ -o ./outputwin/poll.o
	REM %gcc% -c libevent/select.c -Ilibevent/include -Ilibevent/ -o ./outputwin/select.o
	REM %gcc% -c libevent/signal.c -Ilibevent/include -Ilibevent/ -o ./outputwin/signal.o
	REM %gcc% -c libevent/evport.c -Ilibevent/include -Ilibevent/ -o ./outputwin/evport.o
	REM %gcc% -c libevent/devpoll.c -Ilibevent/include -Ilibevent/ -o ./outputwin/devpoll.o
	REM %gcc% -c libevent/log.c -Ilibevent/include -Ilibevent/ -o ./outputwin/log.o
	REM %gcc% -c libevent/evthread.c -Ilibevent/include -Ilibevent/ -o ./outputwin/evthread.o
	REM %gcc% -c libevent/evutil_time.c -Ilibevent/include -Ilibevent/ -o ./outputwin/evutil_time.o
	REM %gcc% -c libevent/kqueue.c -Ilibevent/include -Ilibevent/ -o ./outputwin/kqueue.o
	REM %gcc% -c libevent/evutil.c -Ilibevent/include -Ilibevent/ -o ./outputwin/evutil.o
	REM %gcc% -c libevent/evmap.c -Ilibevent/include -Ilibevent/ -o ./outputwin/evmap.o
	REM %gcc% -c libevent/event.c  -Ilibevent/ -o ./outputwin/event.o
	
	REM %ar% -r outputwin/libevent.a %libeventlibs% 

REM %gcc% server.c  outputwin/libevent.a  outputwin/winhiredis.a  -o outputwin/modbus-tcp-server -I./mxml-release-2.10  -I./hiredis
%gcc% test/testredis.c  libevent.a  outputwin/winhiredis.a  -o outputwin/modbus-tcp-server   -I./hiredis -I./libevent/include 
