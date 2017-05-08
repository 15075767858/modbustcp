# this is make file 
cc=gcc
#cc=E:\Sourcery_CodeBench_Lite_for_ARM_GNU_Linux\bin\arm-none-linux-gnueabi-gcc.exe
libmodbusdir=libmodbus-3.1.4
hiredisdir=hiredis
libs=\
	./output/dict.o \
	./output/net.o \
	./output/read.o \
	./output/sds.o \
	./output/hiredis.o 

2:
	$(cc) $(libs) server.c -o a.out -g
1:
	cc test.c -g && ./a.out
3:
	$(cc) $(libs) server.c -o a.out -g && 	./a.out
run:all
	./server
all:server 
server:server.c 
	$(cc) $(libs) server.c -o server

test: modbus.o hiredis.o
	$(cc)  $(libs) test.c -o test.out 
modbus.o:
	$(cc) -c ${libmodbusdir}/src/modbus.c -I${libmodbusdir}/ -o ./output/modbus.o
	$(cc) -c ${libmodbusdir}/src/modbus-tcp.c -I${libmodbusdir}/ -o ./output/modbus-tcp.o
	$(cc) -c ${libmodbusdir}/src/modbus-data.c -I${libmodbusdir}/ -o ./output/modbus-data.o
hiredis.o:
	$(cc) -c ${hiredisdir}/dict.c -I${hiredisdir}/ -o ./output/dict.o
	$(cc) -c ${hiredisdir}/net.c -I${hiredisdir}/ -o ./output/net.o
	$(cc) -c ${hiredisdir}/read.c -I${hiredisdir}/ -o ./output/read.o
	$(cc) -c ${hiredisdir}/sds.c -I${hiredisdir}/ -o ./output/sds.o
	$(cc) -c ${hiredisdir}/hiredis.c -I${hiredisdir}/ -o ./output/hiredis.o

clean:
	find ./ -name '*.o' | xargs rm 