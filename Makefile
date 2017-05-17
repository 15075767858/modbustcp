
cc=gcc
#cc=arm-none-linux-gnueabi-gcc
libdir=library
hiredisdir=${libdir}/hiredis
odir=output
libeventdir=${libdir}/libevent

mxmllibs=mxml-index.o mxml-private.o mxml-search.o mxml-attr.o mxml-entity.o mxml-get.o mxml-file.o mxml-node.o mxml-set.o mxml-string.o 
libevent=evutil_rand.o epoll.o poll.o select.o signal.o evport.o devpoll.o log.o evthread.o evutil_time.o kqueue.o evmap.o evutil.o event.o
hiredislibs=async.o dict.o net.o read.o sds.o hiredis.o
libs = device.o asynredis.o
src=src/main.c src/device.c src/asynredis.c

asynredis:
	gcc src/asynredis.c src/device.c libevent.a hiredis.a -Ilibrary/hiredis
testthread:
	$(cc) hiredis.a libevent.a src/device.c test/testthread.c  -o a.out  -g  -I./${hiredisdir}/    &&./a.out
testredis:
	$(cc) hiredis.a libevent.a src/device.c test/testredis.c  -o a.out  -g  -I./${hiredisdir}/    &&./a.out
run:
	make all
	./modbus-tcp-server
debug:
	${cc} ${src} hiredis.a libevent.a -I${hiredisdir}  -g
all:
	${cc} ${src} hiredis.a libevent.a -I${hiredisdir} -o modbus-tcp-server 
build:hiredis.a libevent.a ${libs}
	rm ${libevent} ${hiredislibs} ${libs}

asynredis.o:
	$(cc) -c src/asynredis.c -I${hiredisdir}
device.o:
	$(cc) -c src/device.c
hiredis.a: ${hiredislibs}
	ar -r hiredis.a $(hiredislibs) 
async.o:
	$(cc) -c ${hiredisdir}/async.c -I${hiredisdir}
dict.o:
	$(cc) -c ${hiredisdir}/dict.c -I${hiredisdir}
net.o:
	$(cc) -c ${hiredisdir}/net.c -I${hiredisdir}
read.o:
	$(cc) -c ${hiredisdir}/read.c -I${hiredisdir}
sds.o:
	$(cc) -c ${hiredisdir}/sds.c -I${hiredisdir}
hiredis.o:
	$(cc) -c ${hiredisdir}/hiredis.c -I${hiredisdir}

libevent.a:${libevent}
	ar -r libevent.a $(libevent) 
evutil_rand.o:
	${cc} -c ${libeventdir}/evutil_rand.c -I${libeventdir}/include 
epoll.o:
	${cc} -c ${libeventdir}/epoll.c -I${libeventdir}/include 
poll.o:
	${cc} -c ${libeventdir}/poll.c -I${libeventdir}/include 
select.o:
	${cc} -c ${libeventdir}/select.c -I${libeventdir}/include 
signal.o:
	${cc} -c ${libeventdir}/signal.c -I${libeventdir}/include 
evport.o:
	${cc} -c ${libeventdir}/evport.c -I${libeventdir}/include 
devpoll.o:
	${cc} -c ${libeventdir}/devpoll.c -I${libeventdir}/include 
log.o:
	${cc} -c ${libeventdir}/log.c -I${libeventdir}/include 
evthread.o:
	${cc} -c ${libeventdir}/evthread.c -I${libeventdir}/include 
evutil_time.o:
	${cc} -c ${libeventdir}/evutil_time.c -I${libeventdir}/include 
kqueue.o:
	${cc} -c ${libeventdir}/kqueue.c -I${libeventdir}/include 
evmap.o:
	${cc} -c ${libeventdir}/evmap.c -I${libeventdir}/include 
evutil.o:
	${cc} -c ${libeventdir}/evutil.c -I${libeventdir}/include 
event.o:
	${cc} -c ${libeventdir}/event.c -I${libeventdir}/include 