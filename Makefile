# this is make file 
cc=gcc
#cc=E:\Sourcery_CodeBench_Lite_for_ARM_GNU_Linux\bin\arm-none-linux-gnueabi-gcc.exe
libmodbusdir=libmodbus-3.1.4
hiredisdir=hiredis
mxmldir=mxml-release-2.10
libxmldir=libxml2-2.9.4
libeventdir=libevent
mxmllibs=\
	./output/mxml-index.o \
	./output/mxml-private.o \
	./output/mxml-search.o \
	./output/mxml-attr.o \
	./output/mxml-entity.o \
	./output/mxml-get.o \
	./output/mxml-file.o \
	./output/mxml-node.o \
	./output/mxml-set.o \
	./output/mxml-string.o \

hiredislibs=\
	./output/async.o \
	./output/dict.o \
	./output/net.o \
	./output/read.o \
	./output/sds.o \
	./output/hiredis.o 

libevent=\
	./output/evutil_rand.o \
	./output/epoll.o \
	./output/poll.o \
	./output/select.o \
	./output/signal.o \
	./output/evport.o \
	./output/devpoll.o \
	./output/log.o \
	./output/evthread.o \
	./output/evutil_time.o \
	./output/kqueue.o \
	./output/evmap.o \
	./output/evutil.o \
	./output/event.o 
2:
	$(cc) $(hiredislibs) server.c -o a.out -g
1:
	cc test.c -g && ./a.out
server:
	$(cc) $(hiredislibs)  server.c -o a.out -g && 	./a.out
run:all
	./server
all:server 
test:  hiredis.a  mxmllibs.a
	$(cc)  $(libs) test.c -o test.out
test1: hiredis.a  mxmllibs.a
	$(cc)  $(hiredislibs)  $(mxmllibs) test.c  -o a.out  -g   -I./${libxmldir}/include
test2: hiredis.a  
	$(cc)  $(hiredislibs)  test/test2.c  -o a.out  -g   -I./${libxmldir}/include &&./a.out
testredis: hiredis.a
	$(cc)  $(hiredislibs) $(libevent)   test/testredis.c  -o a.out  -g  -I./${hiredisdir}/    &&./a.out
testthread: hiredis.a
	$(cc)  $(hiredislibs)  test/testthread.c  -o a.out  -g  -I./${hiredisdir}/    &&./a.out
hiredis.a:
	ar -r hiredis.a $(hiredislibs) 
mxmllibs.a:
	ar -r mxmllibs.a $(mxmllibs)
hiredis.o:
	$(cc) -c ${hiredisdir}/dict.c -I${hiredisdir}/ -o ./output/dict.o
	$(cc) -c ${hiredisdir}/net.c -I${hiredisdir}/ -o ./output/net.o
	$(cc) -c ${hiredisdir}/read.c -I${hiredisdir}/ -o ./output/read.o
	$(cc) -c ${hiredisdir}/sds.c -I${hiredisdir}/ -o ./output/sds.o
	$(cc) -c ${hiredisdir}/hiredis.c -I${hiredisdir}/ -o ./output/hiredis.o
	$(cc) -c ${hiredisdir}/async.c -I${hiredisdir}/ -o ./output/async.o
mxml.o:
	$(cc) -c ${mxmldir}/mxml-search.c -I${mxmldir}/ -o ./output/mxml-search.o
	$(cc) -c ${mxmldir}/mxml-attr.c -I${mxmldir}/ -o ./output/mxml-attr.o
	$(cc) -c ${mxmldir}/mxml-entity.c -I${mxmldir}/ -o ./output/mxml-entity.o
	$(cc) -c ${mxmldir}/mxml-get.c -I${mxmldir}/ -o ./output/mxml-get.o
	$(cc) -c ${mxmldir}/mxml-node.c -I${mxmldir}/ -o ./output/mxml-node.o
	$(cc) -c ${mxmldir}/mxml-set.c -I${mxmldir}/ -o ./output/mxml-set.o
	$(cc) -c ${mxmldir}/mxml-string.c -I${mxmldir}/ -o ./output/mxml-string.o
	$(cc) -c ${mxmldir}/mxml-file.c -I${mxmldir}/ -o ./output/mxml-file.o
	$(cc) -c ${mxmldir}/mxml-private.c -I${mxmldir}/ -o ./output/mxml-private.o
	$(cc) -c ${mxmldir}/mxml-index.c -I${mxmldir}/ -o ./output/mxml-index.o
libevent.o:
	$(cc) -c libevent/event.c -Ilibevent/include -Ilibevent/ -o ./output/event.o
	$(cc) -c libevent/evutil_rand.c -Ilibevent/include -Ilibevent/ -o ./output/evutil_rand.o
	$(cc) -c libevent/epoll.c -Ilibevent/include -Ilibevent/ -o ./output/epoll.o
	$(cc) -c libevent/poll.c -Ilibevent/include -Ilibevent/ -o ./output/poll.o
	$(cc) -c libevent/select.c -Ilibevent/include -Ilibevent/ -o ./output/select.o
	$(cc) -c libevent/signal.c -Ilibevent/include -Ilibevent/ -o ./output/signal.o
	$(cc) -c libevent/evport.c -Ilibevent/include -Ilibevent/ -o ./output/evport.o
	$(cc) -c libevent/devpoll.c -Ilibevent/include -Ilibevent/ -o ./output/devpoll.o
	$(cc) -c libevent/log.c -Ilibevent/include -Ilibevent/ -o ./output/log.o
	$(cc) -c libevent/evthread.c -Ilibevent/include -Ilibevent/ -o ./output/evthread.o
	$(cc) -c libevent/evutil_time.c -Ilibevent/include -Ilibevent/ -o ./output/evutil_time.o
	$(cc) -c libevent/kqueue.c -Ilibevent/include -Ilibevent/ -o ./output/kqueue.o
	$(cc) -c libevent/evutil.c -Ilibevent/include -Ilibevent/ -o ./output/evutil.o
	$(cc) -c libevent/evmap.c -Ilibevent/include -Ilibevent/ -o ./output/evmap.o

clean:
	find ./ -name '._*' | xargs rm
	find ./ -name '*.o' | xargs rm 
	find ./ -name '*.a' | xargs rm 
clean-temp:
	find ./ -name '._*' | xargs rm
clean-a:
	find ./ -name '*.a' | xargs rm 
clean-o:
	find ./ -name '*.o' | xargs rm 