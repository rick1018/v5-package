dirs := \
	im_osal \
	omx_component/common/

subdirs:
	$(foreach dir, $(dirs), $(MAKE) -C $(dir) all || exit "$$?";)

install:

all: subdirs install

clean:
	echo "clean"
