BUILD_SUBDIRS = threads userprog vm filesys
TAR_PATH := team${TEAM}.tar.gz

all: threads userprog vm filesys

CLEAN_SUBDIRS = $(BUILD_SUBDIRS)

threads::
	$(MAKE) -C threads

userprog::
	$(MAKE) -C userprog

vm::
	$(MAKE) -C vm

filesys::
	$(MAKE) -C filesys

clean::
	for d in $(CLEAN_SUBDIRS); do $(MAKE) -C $$d $@; done
	rm -f TAGS tags
	rm -f *.tar.gz

distclean:: clean
	find . -name '*~' -exec rm '{}' \;

TAGS_SUBDIRS = $(BUILD_SUBDIRS) devices lib include
TAGS_SOURCES = find $(TAGS_SUBDIRS) -name \*.[chS] -print

TAGS::
	etags `$(TAGS_SOURCES)`

tags::
	ctags `$(TAGS_SOURCES)`

cscope.files::
	$(TAGS_SOURCES) > cscope.files

cscope:: cscope.files
	cscope -b -q -k

archive:: clean
ifeq ($(shell echo ${TEAM} | egrep "^[1-9]{1}[0-9]{0,2}$$"),)
	@echo "Check your team number: $(TEAM)"
else
	@tar -zcf /tmp/${TAR_PATH} . && \
		mv /tmp/${TAR_PATH} . && \
		echo "Successfully archived. Submit '${TAR_PATH}'."
endif
