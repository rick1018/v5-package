dirs := \
	linux/dwl \
	linux/avs \
	linux/h264high \
	linux/jpeg  \
	linux/mpeg2 \
	linux/mpeg4 \
	linux/pp \
	linux/rv \
	linux/vc1 \
	linux/vp6 \
	linux/vp8

subdirs:
	$(foreach dir, $(dirs), $(MAKE) -C $(dir) all || exit "$$?";)

install:
	-mkdir -p $(CONFIG_PREFIX)/include
	-cp source/inc/*.h $(CONFIG_PREFIX)/include/

all: subdirs install

clean:
	echo "clean"
