##############################################################################################
## Description: make all the mongoose source files.
## History
## Version		Date		Author		Description
## V1.0		  2019.03.28    lomboswer		Create File.
##############################################################################################

SRCS=$(wildcard *.c)
OBJS=$(SRCS:%.c=%.o)
DEP:=$(SRCS:%.c=%.d)
CFLAGS+=-fPIC -fstack-protector --param=ssp-buffer-size=4 -Wformat -Wformat-security -Wl,-z,relro,-z,now -Wl,--as-needed -Wl,--no-copy-dt-needed-entries -Wl,-z,noexecstatck -Wl,-z,noexecheap -w -g
CFLAGS+= -DME_DEBUG=1 -D_REENTRANT -DPIC  -DME_COM_COMPILER=1 -DME_COM_LIB=1 -DME_COM_MATRIXSSL=0 \
		-DME_COM_MBEDTLS=1 -DME_COM_NANOSSL=0 -DME_COM_OPENSSL=0 -DME_COM_OSDEP=1 -DME_COM_SSL=1 \
		-DME_COM_VXWORKS=0  -DME_DEBUG -D_FILE_OFFSET_BITS=64 -D_FILE_OFFSET_BITS=64 \
		-DMBEDTLS_USER_CONFIG_FILE=\"embedtls.h\"

.PHONY: all clean

all: $(OBJS)
	$(AR) $(AFLAGS) $(LIBS_PATH)/libgoahead.a $(OBJS)

-include $(DEP)
$(OBJS):%.o:%.c
	$(CC) $(CFLAGS) -MM -MT $@ -MF $(patsubst %.o, %.d, $@) $<
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	-$(RM) *.o
	-$(RM) *.d
	$(RM) $(LIBS_PATH)/libgoahead.a
