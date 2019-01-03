TARGET          = remote_control
				   
SRCS := con_test.c uart.c socket.c tcpServer.c

OBJS    = $(SRCS:.c=.o)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) $(LIBS) $(LD_FLAG) -o $@

all:$(TARGET) install

install:
	mkdir -p $(CONFIG_PREFIX)/usr/bin
	cp $(TARGET) $(CONFIG_PREFIX)/usr/bin

clean:
	rm -rf $(TARGET) *.o *~
