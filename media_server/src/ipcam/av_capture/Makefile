SUBDIRS =libcgi libmsgdrv libstreamdrv

CLEANSUBDIRS = $(addsuffix .clean, $(SUBDIRS))

.PHONY: $(SUBDIRS) $(DEBUGSUBDIRS) $(RELEASESUBDIRS) $(INSTALLSUBDIRS) \
	$(CLEANSUBDIRS)	

all: $(SUBDIRS) 

clean: $(CLEANSUBDIRS)	
	rm -rf *.o *.gdb 
	rm -rf $(SERVER_BIN) $(CLIENT_BIN) $(CAPTURE_BIN)

$(SUBDIRS):
	@echo
	@echo Making all in subdirectory $@...
	@$(MAKE) -C $@	
	
$(CLEANSUBDIRS):
	@cd $(basename $@) ; $(MAKE) clean

