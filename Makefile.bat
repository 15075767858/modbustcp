set gcc=E:\Sourcery_CodeBench_Lite_for_ARM_GNU_Linux\bin\arm-none-linux-gnueabi-gcc.exe
set libmodbusdir=libmodbus-3.1.4
set hiredisdir=hiredis
    REM %gcc% -c %libmodbusdir%/src/modbus.c -I%libmodbusdir%/ -o ./output/modbus.o
	REM %gcc% -c %libmodbusdir%/src/modbus-tcp.c -I%libmodbusdir%/ -o ./output/modbus-tcp.o
	REM %gcc% -c %libmodbusdir%/src/modbus-data.c -I%libmodbusdir%/ -o ./output/modbus-data.o
    REM %gcc% -c %hiredisdir%/dict.c -I%hiredisdir%/ -o ./output/dict.o
	REM %gcc% -c %hiredisdir%/net.c -I%hiredisdir%/ -o ./output/net.o
	REM %gcc% -c %hiredisdir%/read.c -I%hiredisdir%/ -o ./output/read.o
	REM %gcc% -c %hiredisdir%/sds.c -I%hiredisdir%/ -o ./output/sds.o
	REM %gcc% -c %hiredisdir%/hiredis.c -I%hiredisdir%/ -o ./output/hiredis.o
%gcc% test.c ./output/modbus.o ./output/modbus-tcp.o ./output/modbus-data.o ./output/dict.o ./output/net.o ./output/read.o ./output/sds.o ./output/hiredis.o  -o test.out -I./libmodbus-3.1.4/src  -I./hiredis