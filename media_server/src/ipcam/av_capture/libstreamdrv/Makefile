INCLUDE=-I../include -I$(APP_INC) -I.

LIBNAME=../lib/libstreamdrv

all: libstreamdrv.so

libstreamdrv.so: mem_mng.c cache_mng.c semaphore_util.c gop_lock.c stream.c nalu.c
	$(CC) $(INCLUDE) -Wall -O3 -shared -fPIC -o $@ $^ $(LDFLAGS)
	mkdir -p ../../lib
	cp -a libstreamdrv.so ../../lib/

clean:
	rm -f *.o core ${LIBNAME}.a ${LIBNAME}.so
