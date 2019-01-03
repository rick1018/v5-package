#!/bin/bash
#create tag

#cvs tag $1 EncGetOption.c
#cvs tag $1 EncGetOption.h
cvs tag $1 GetNextOpt.c
cvs tag $1 GetNextOpt.h
cvs tag $1 JpegTestBench.c
cvs tag $1 JpegTestBench.h
cvs tag $1 Makefile
#cvs tag $1 armscript.txt
#cvs tag $1 statcheck.sh
#cvs tag $1 tagger.sh
cvs tag $1 test_jpeg.sh
#cvs tag $1 TestBenchInternals.c
cvs tag $1 checkall_jpeg.sh
cvs tag $1 checkcase_jpeg.sh
cvs tag $1 checkall_csv.sh
cvs tag $1 checkcase_csv.sh
cvs tag $1 README

cvs tag $1 ../../../source/inc/basetype.h \
           ../../../source/inc/jpegencapi.h \
	       ../../../source/inc/ewl.h 

cvs tag $1 ../../../source/common/encasiccontroller.c \
            ../../../source/common/encasiccontroller.h \
            ../../../source/common/encasiccontroller_v2.c \
            ../../../source/common/enccfg.h \
            ../../../source/common/encpreprocess.c \
            ../../../source/common/encpreprocess.h \
            ../../../source/common/enccommon.h
            
cvs tag $1 ../../../source/jpeg 

cvs tag $1  ../../Makefile
cvs tag $1  ../../ewl
cvs tag $1  ../../kernel_module
cvs tag $1  ../../memalloc

cvs tag $1  ../../debug_trace/encdebug.h
cvs tag $1  ../../debug_trace/enctrace.c
cvs tag $1  ../../debug_trace/enctrace.h
cvs tag $1  ../../debug_trace/enctracestream.h

