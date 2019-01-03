# this is for versatile platform
CROSS_COMPILE=arm-none-linux-gnueabi-
CC = $(CROSS_COMPILE)gcc
CFLAGS = -O -g -Wall -march=armv5 -I../../../source/inc

LIB = ../../lib8270enc.a

TESTBENCH=integration_testbench

all: IntegrationTestRegister.o
	$(CC) $(CFLAGS) $^ -o $(TESTBENCH) $(LIB)
         
lib:
	make -C ../.. versatile

clean:
	-rm -f *.o $(TESTBENCH)
        
        
