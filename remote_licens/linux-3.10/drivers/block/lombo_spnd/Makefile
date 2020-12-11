
lombo_spnd-objs := spnd.o blockdev.o debug.o libcommon.a libspndphy.a libnftl.a

$(obj)/libcommon.a: $(src)/libs/libcommon .FORCE
	cp $< $@

$(obj)/libspndphy.a: $(src)/libs/libspndphy .FORCE
	cp $< $@

$(obj)/libnftl.a: $(src)/libs/libnftl .FORCE
	cp $< $@

SOURCE := $(srctree)/$(src)/libs/libspndburn
ifneq ($(wildcard $(SOURCE)), )
ccflags-y += -DSPND_HAS_BURN_LIB
lombo_spnd-objs += libspndburn.a
$(obj)/libspndburn.a: $(src)/libs/libspndburn .FORCE
	cp $< $@
endif

SOURCE := $(srctree)/$(src)/libs/libspnddump
ifneq ($(wildcard $(SOURCE)), )
ccflags-y += -DSPND_HAS_DUMP_LIB
lombo_spnd-objs += libspnddump.a
$(obj)/libspnddump.a: $(src)/libs/libspnddump .FORCE
	cp $< $@
endif

obj-$(CONFIG_LOMBO_SPND) += lombo_spnd.o

SOURCE := $(srctree)/$(src)/extra/extra.c
ifneq ($(wildcard $(SOURCE)), )
ccflags-y += -DSPND_HAS_EXTRA_CODE
endif

SOURCE := $(srctree)/$(src)/extra/spndphy_test/spndphy_test.c
ifneq ($(wildcard $(SOURCE)), )
obj-y += extra/spndphy_test/
endif

#ccflags-y += -DDEBUG

 .FORCE:
