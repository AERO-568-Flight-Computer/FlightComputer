#
#	FILE NAME: Makefile
#
#	FILE DESCRIPTION: Make description file for building example programs
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

CC=gcc
DEBUG_FLAGS=-g
INCLUDE_FLAGS=-I../include
LIBRARY_FLAGS=-L../lib -lrtd-dm35424 -lpthread -lm
OPTIMIZE_FLAGS=-O6
WARNING_FLAGS=-Wall
CFLAGS:=$(DEBUG_FLAGS) $(INCLUDE_FLAGS) $(OPTIMIZE_FLAGS) $(WARNING_FLAGS)

EXAMPLES = \
	dm35424_adc \
	dm35424_adc_continuous_dma \
	dm35424_dac \
	dm35424_dac_dma \
	dm35424_dio \
	dm35424_temperature \
	dm35424_ref_adjust \
	dm35424_list_fb 


all:	$(EXAMPLES)

dm35424_adc_for_cpopa:	dm35424_adc_for_cpopa.c udp_send_data.c
	$(CC) $(CFLAGS) -o dm35424_adc_for_cpopa dm35424_adc_for_cpopa.c udp_send_data.c \
		$(LIBRARY_FLAGS)

clean:
	rm -f *.o *.dat *~ $(EXAMPLES) $(TESTS)
