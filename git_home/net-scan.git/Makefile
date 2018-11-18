#
#	Copyright (C) 2007-2017 Delta Networks Inc.
#

CC = gcc

FILES = main.o arp.o bios.o dni_system.o util.o cJSON.o
FILES2 = devscan.o bios.o
FILES3 = UA_Parser.o util.o cJSON.o
FILES4 = devices_info.o util.o cJSON.o

CFLAGS += -Wall -Wunused -g -O2

all : net-scan dev-scan UA_Parser devices_info

net-scan : $(FILES)
	$(CC) $(CFLAGS) $(FILES) -o $@ -lconfig -lm
	@echo "make $@ finished on `date`"

dev-scan : $(FILES2)
	$(CC) $(CFLAGS) $(FILES2) -o $@
	@echo "make $@ finished on `date`"

UA_Parser : $(FILES3)
	$(CC) $(CFLAGS) $(FILES3) -o $@
	@echo "make $@ finished on `date`"

devices_info : $(FILES4)
	$(CC) $(CFLAGS) $(FILES4) -o $@
	@echo "make $@ finished on `date`"

%.o : %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o 
	rm -f net-scan dev-scan UA_Parser devices_info
