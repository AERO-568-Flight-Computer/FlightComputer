#
#	FILE NAME: Makefile
#
#	FILE DESCRIPTION: Make description file for building driver
#
#	PROJECT NAME: Linux Software (DM35424)
#
#	PROJECT VERSION: (Defined in README.TXT)
#
#	This file and its contents are copyright (C) RTD Embedded Technologies,
#	Inc.  All Rights Reserved.
#
#	This software is licensed as described in the RTD End-User Software
#	License Agreement.  For a copy of this agreement, refer to the file
#	LICENSE.TXT (which should be included with this software) or contact RTD
#	Embedded Technologies, Inc.
#


MY_CFLAGS := -I$(shell pwd)/../include -g

#
# Kernel build environment directory.  Supposedly it is safer to use this
# method of referring to it than using /usr/src.
#

KERNEL_DIR := /lib/modules/$(shell uname -r)/build

KERNEL_VER := $(shell uname -r | cut -c1-3)
ifeq (4.0, $(firstword $(sort $(KERNEL_VER) 4.0)))
SUBDIRS_OR_M := M
else
SUBDIRS_OR_M := SUBDIRS
endif


obj-m := rtd-dm35424.o

DRIVER_MODULE=rtd-dm35424.ko
FILES_TO_CLEAN= \
	*.o \
	rtd-dm35424.ko \
	.*.cmd \
	*.ko \
	.tmp_versions \
	rtd-dm35424.mod.c \
	rtd-dm35424.mod \
	.*.d \
	*.symvers \
	*.order \
    *.markers \
	*~

driver:		rtd-dm35424.c
	make -C $(KERNEL_DIR) $(SUBDIRS_OR_M)=`pwd` EXTRA_CFLAGS="$(MY_CFLAGS)" modules

.PHONY: debug
debug:
	make -C $(KERNEL_DIR) $(SUBDIRS_OR_M)=`pwd` EXTRA_CFLAGS="$(MY_CFLAGS) -DDEBUG" modules

.PHONY: debug_int
debug_int:
	make -C $(KERNEL_DIR) $(SUBDIRS_OR_M)=`pwd` EXTRA_CFLAGS="$(MY_CFLAGS) -DDEBUG_INT" modules

.PHONY: debug_dma
debug_dma:
	make -C $(KERNEL_DIR) $(SUBDIRS_OR_M)=`pwd` EXTRA_CFLAGS="$(MY_CFLAGS) -DDEBUG_DMA" modules

.PHONY: debug_all
debug_all:
	make -C $(KERNEL_DIR) $(SUBDIRS_OR_M)=`pwd` EXTRA_CFLAGS="$(MY_CFLAGS) -DDEBUG_ALL" modules


clean:
	rm -rf $(FILES_TO_CLEAN)

load:
	insmod ./$(DRIVER_MODULE)
	@sleep 1
	chmod 666 /dev/rtd-dm35424-*

unload:
	rmmod rtd-dm35424
	@echo "Removing old device files ..."; \
	rm -f /dev/rtd-dm35424-* \
	
