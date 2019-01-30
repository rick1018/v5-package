INCLUDES += -I./ -I$(APP_INC)

C_FLAGS += -Wall -g -O3 
C_FLAGS += -fPIC
AR_FLAGS += -r
SO_LFLAGS += -shared -lpthread

CC1 = $(CROSS_COMPILE)gcc $(INCLUDES) $(C_FLAGS) -c
STRIP = $(CROSS_COMPILE)strip
LD = $(CROSS_COMPILE)ld
AR = $(CROSS_COMPILE)ar
GCCLD = $(CROSS_COMPILE)gcc

REL_EXE1 = reboot_ipnc

REL_LIB1 = file_msg_drv.a
REL_SLIB1 = libfilemsgdrv.so

REL_LIB3 = share_mem.a
REL_SLIB3 = libsharemem.so

REL_LIB5 = msg_util.a
REL_SLIB5 = libmsgutil.so

REL_LIB7 = avdata_drv.a
REL_SLIB7 = libavdatadrv.so

REL_LIB9 = sem_util.a
REL_SLIB9 = libsemutil.so


REL_OBJ1 = file_msg_drv.o 

REL_OBJ3 = share_mem.o

REL_OBJ5 = msg_util.o

REL_OBJ7 = stream_msg_drv.o avdata_drv.o avilib.o  

REL_OBJ9 = sem_util.o

all: $(REL_SLIB1) $(REL_SLIB3) $(REL_SLIB5) $(REL_SLIB7) $(REL_SLIB9) install

#$(REL_LIB1): $(REL_OBJ1)
#	$(AR) $(AR_FLAGS) $(REL_LIB1) $(REL_OBJ1)


$(REL_SLIB1): file_msg_drv.c $(APP_INC)/file_msg_drv.h $(APP_INC)/Msg_Def.h $(APP_INC)/sys_env_type.h
	$(CC) $(INCLUDES) $(C_FLAGS) $(SO_LFLAGS) -o $@ $<


$(REL_SLIB3): share_mem.c $(APP_INC)/share_mem.h
	$(CC) $(INCLUDES) $(C_FLAGS) $(SO_LFLAGS) -o $@ $<


$(REL_SLIB5): msg_util.c $(APP_INC)/Msg_Def.h
	$(CC) $(INCLUDES) $(C_FLAGS) $(SO_LFLAGS) -o $@ $<

$(REL_SLIB7):
	$(CC) $(INCLUDES) $(C_FLAGS) $(SO_LFLAGS) -o $@ stream_msg_drv.c avdata_drv.c avilib.c $<

$(REL_SLIB9): sem_util.c $(APP_INC)/sem_util.h
	$(CC) $(INCLUDES) $(C_FLAGS) $(SO_LFLAGS) -o $@ $<


file_msg_drv.o : file_msg_drv.c $(APP_INC)/file_msg_drv.h $(APP_INC)/Msg_Def.h\
		$(APP_INC)/sys_env_type.h
	$(CC) $(INCLUDES) $(C_FLAGS) -o $@ $<

alarm_msg_drv.o : alarm_msg_drv.c $(APP_INC)/alarm_msg_drv.h \
		$(APP_INC)/Msg_Def.h $(APP_INC)/sys_env_type.h
	$(CC) $(INCLUDES) $(C_FLAGS) -o $@ $<

share_mem.o: share_mem.c $(APP_INC)/share_mem.h
	$(CC) $(INCLUDES) $(C_FLAGS) -o $@ $<

sys_msg_drv.o : sys_msg_drv.c $(APP_INC)/sys_msg_drv.h $(APP_INC)/Msg_Def.h\
		$(APP_INC)/sys_env_type.h $(APP_INC)/share_mem.h
	$(CC) $(INCLUDES) $(C_FLAGS) -o $@ $<

msg_util.o: msg_util.c $(APP_INC)/Msg_Def.h
	$(CC) $(INCLUDES) $(C_FLAGS) -o $@ $<

stream_msg_drv.o: stream_msg_drv.c $(APP_INC)/stream_msg_drv.h
	$(CC) $(INCLUDES) $(C_FLAGS) -o $@ $<

avdata_drv.o: avdata_drv.c $(APP_INC)/avdata_drv.h
	$(CC) $(INCLUDES) $(C_FLAGS) -o $@ $<

sem_util.o: sem_util.c $(APP_INC)/sem_util.h
	$(CC) $(INCLUDES) $(C_FLAGS) -o $@ $<

sysctrl.o: sysctrl.c $(APP_INC)/sysctrl.h $(APP_INC)/sys_env_type.h
	$(CC) $(INCLUDES) $(C_FLAGS) -o $@ $<

net_config.o: net_config.c $(APP_INC)/net_config.h
	$(CC) $(INCLUDES) $(C_FLAGS) -o $@ $<

system_call_msg_drv.o: system_call_msg_drv.c $(APP_INC)/system_call_msg_drv.h $(APP_INC)/Msg_Def.h $(APP_INC)/sys_env_type.h
	$(CC) $(INCLUDES) $(C_FLAGS) -o $@ $<

sqlite3_util.o: sqlite3_util.c $(APP_INC)/sqlite3_util.h
	$(CC) $(INCLUDES) $(C_FLAGS) -o $@ $<

reboot_ipnc:
	$(CC) $(C_FLAGS) -o reboot_ipnc reboot_ipnc.c
	$(STRIP) reboot_ipnc

clean:
	-$(RM) -f *.o *.so
	-$(RM) -f *.a
	-$(RM) -f $(REL_EXE1)

install:
	install -d $(APP_LIB)

	install $(REL_SLIB1) $(APP_LIB)
	install $(REL_SLIB3) $(APP_LIB)
	install $(REL_SLIB5) $(APP_LIB)
	install $(REL_SLIB7) $(APP_LIB)
	install $(REL_SLIB9) $(APP_LIB)

	install -d $(APP_SO_LIB)
	install $(REL_SLIB1) $(APP_SO_LIB)
	install $(REL_SLIB3) $(APP_SO_LIB)
	install $(REL_SLIB5) $(APP_SO_LIB)
	install $(REL_SLIB7) $(APP_SO_LIB)
	install $(REL_SLIB9) $(APP_SO_LIB)
