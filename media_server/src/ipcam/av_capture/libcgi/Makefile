#   Copyright (c) 1998,9 by Martin Schulze <joey@infodrom.north.de>

#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.

#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.

#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111, USA.

#CROSS_COMPILE = /opt/mv_pro_5.0/montavista/pro/devkit/arm/v5t_le/bin/arm_v5t_le-
#CROSS_AR = $(CROSS_COMPILE)
#CROSS_RANLIB = $(CROSS_COMPILE)

CC=$(CROSS_COMPILE)gcc
AR=$(CROSS_COMPILE)ar
RANLIB=$(CROSS_COMPILE)ranlib

CFLAGS = -I. -I../include -Wall -O2 -g
LDFLAGS = -L.
LIBS = -lcgi

OBJS = HttpPacket.o HttpRequest.o HttpResponse.o base64.o
#OBJS = cgi.o cookies.o 

libcgi.a: $(OBJS)
	@$(AR) rc $@ $^	
#	@cp -f libcgi.a ../lib

cgitest: cgitest.o libcgi.a
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

cli: cli.o libcgi.a
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)
	
cgidate: cgidate.o libcgi.a
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)	

test: test.o libcgi.a
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

jumpto: jumpto.o libcgi.a
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

all: libcgi.a cgitest jumpto 
	mkdir -p ../include
	mkdir -p ../lib
	@cp -f libcgi.a ../lib

install: cgitest
	mkdir -p bin
	install -m 755 cgitest ./bin

clean:
	rm -f cgitest test cgitest.o jumpto jumpto.o libcgi.a $(OBJS) *.o *.[35].html *.exe

htmlman:
	for f in *.[35]; do \
	  man -l $$f|rman -f HTML --title $$f -r "%s.%s.html" > $$f.html; \
	done
