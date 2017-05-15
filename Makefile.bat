REM set gcc=E:\Sourcery_CodeBench_Lite_for_ARM_GNU_Linux\bin\arm-none-linux-gnueabi-gcc.exe
set gcc=arm-none-linux-gnueabi-gcc.exe
set ar=arm-none-linux-gnueabi-ar.exe
set hiredisdir=hiredis
set mxmldir=mxml-release-2.10
set libevetdir=libevent
set libeventlibs = ./outputwin/evutil_rand.o ./outputwin/epoll.o ./outputwin/poll.o ./outputwin/select.o ./outputwin/signal.o ./outputwin/evport.o ./outputwin/devpoll.o ./outputwin/log.o ./outputwin/evthread.o ./outputwin/evutil_time.o ./outputwin/kqueue.o ./outputwin/evmap.o ./outputwin/evutil.o ./outputwin/event.o
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
	
REM %gcc% server.c   winmxmllibs.a  winhiredis.a  -o modbus-tcp-server -I./mxml-release-2.10  -I./hiredis

	%gcc% -c %libeventdir%/evutil_rand.c -I%libeventdir%/ -o ./outputwin/evutil_rand.o
	%gcc% -c %libeventdir%/epoll.c -I%libeventdir%/ -o ./outputwin/epoll.o
	%gcc% -c %libeventdir%/poll.c -I%libeventdir%/ -o ./outputwin/poll.o
	%gcc% -c %libeventdir%/select.c -I%libeventdir%/ -o ./outputwin/select.o
	%gcc% -c %libeventdir%/signal.c -I%libeventdir%/ -o ./outputwin/signal.o
	%gcc% -c %libeventdir%/evport.c -I%libeventdir%/ -o ./outputwin/evport.o
	%gcc% -c %libeventdir%/devpoll.c -I%libeventdir%/ -o ./outputwin/devpoll.o
	%gcc% -c %libeventdir%/log.c -I%libeventdir%/ -o ./outputwin/log.o
	%gcc% -c %libeventdir%/evthread.c -I%libeventdir%/ -o ./outputwin/evthread.o
	%gcc% -c %libeventdir%/evutil_time.c -I%libeventdir%/ -o ./outputwin/evutil_time.o
	%gcc% -c %libeventdir%/kqueue.c -I%libeventdir%/ -o ./outputwin/kqueue.o
	%gcc% -c %libeventdir%/evmap.c -I%libeventdir%/ -o ./outputwin/evmap.o
	%gcc% -c %libeventdir%/evutil.c -I%libeventdir%/ -o ./outputwin/evutil.o
	%gcc% -c %libeventdir%/event.c -I%libeventdir%/ -o ./outputwin/event.o
	REM %ar% -r winmxmllibs.a  %mxmllibs%	
	REM %ar% -r winhiredis.a %hiredislibs%
	%ar% -r libevent.a %libeventlibs%
%gcc% server.c    winhiredis.a  -o modbus-tcp-server -I./mxml-release-2.10  -I./hiredis


