#
# Makefile for aescrypt
# Copyright (C) 2007-2017
# Paul E. Jones <paulej@packetizer.com>
#
# This software is licensed as "freeware."  Permission to distribute
# this software in source and binary forms is hereby granted without a
# fee.  THIS SOFTWARE IS PROVIDED 'AS IS' AND WITHOUT ANY EXPRESSED OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
# THE AUTHOR SHALL NOT BE HELD LIABLE FOR ANY DAMAGES RESULTING FROM THE
# USE OF THIS SOFTWARE, EITHER DIRECTLY OR INDIRECTLY, INCLUDING, BUT
# NOT LIMITED TO, LOSS OF DATA OR DATA BEING RENDERED INACCURATE.
#

CC=gcc
CFLAGS=-Wall -Wextra -pedantic -std=c99 -D_FILE_OFFSET_BITS=64
LIBS=
AESCRYPT_OBJS=aescrypt.o aes.o sha256.o password.o keyfile.o
KEYGEN_OBJS=aescrypt_keygen.o keyfile.o password.o

#
# Uncomment the line below to compile on Mac and FreeBSD when you see an error about
# not finding the libiconv_* functions when linking
#
#LIBS=-liconv

all: aescrypt aescrypt_keygen

aescrypt: $(AESCRYPT_OBJS)
	$(CC) $(CFLAGS) $(LIBS) -o $@ $(AESCRYPT_OBJS)

aescrypt_keygen: $(KEYGEN_OBJS)
	$(CC) $(CFLAGS) $(LIBS) -o $@ $(KEYGEN_OBJS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $*.c

install: aescrypt
	install -o root -g root -m 755 aescrypt /usr/bin
	install -o root -g root -m 755 aescrypt_keygen /usr/bin

uninstall:
	rm -f /usr/bin/aescrypt
	rm -f /usr/bin/aescrypt_keygen

clean:
	rm -f *.o aescrypt aescrypt_keygen test* *test

test: aescrypt
	@$(CC) -DTEST -o sha.test sha256.c
	@./sha.test
	@rm sha.test
	@$(CC) -DTEST -o aes.test aes.c
	@./aes.test
	@rm aes.test
	# Encrypting and decrypting text files
	# Test zero-length file
	@cat /dev/null > test.orig.txt
	@./aescrypt -e -p "praxis" test.orig.txt
	@cp test.orig.txt.aes test.txt.aes
	@./aescrypt -d -p "praxis" test.txt.aes
	@cmp test.orig.txt test.txt
	@rm test.orig.txt test.orig.txt.aes test.txt.aes test.txt
	# Testing short file (one AES block)
	@echo "Testing..." > test.orig.txt
	@./aescrypt -e -p "praxis" test.orig.txt
	@cp test.orig.txt.aes test.txt.aes
	@./aescrypt -d -p "praxis" test.txt.aes
	@cmp test.orig.txt test.txt
	@rm test.orig.txt test.orig.txt.aes test.txt.aes test.txt
	# Test password length boundary
	# Test password length 0
	@cat /dev/null >test.passwd.txt
	@echo "Testing..." > test.txt
	@# Expecting a failure here, but reflect opposite result code
	@./aescrypt -e -p `cat test.passwd.txt` test.txt 2>/dev/null && \
	    echo Password length test failed && \
	    exit 1 || \
	    true
	@rm test.txt test.passwd.txt
	# Test password length 1023
	@cat /dev/null >test.passwd.txt
	@for x in `seq 1 1023`; do echo -n X >>test.passwd.txt; done
	@echo "Testing..." > test.txt
	@./aescrypt -e -p `cat test.passwd.txt` test.txt
	@rm test.txt.aes test.txt test.passwd.txt
	# Test password length 1024
	@cat /dev/null >test.passwd.txt
	@for x in `seq 1 1024`; do echo -n X >>test.passwd.txt; done
	@echo "Testing..." > test.txt
	@./aescrypt -e -p `cat test.passwd.txt` test.txt
	@rm test.txt.aes test.txt test.passwd.txt
	# Test password length 1025
	@cat /dev/null >test.passwd.txt
	@for x in `seq 1 1025`; do echo -n X >>test.passwd.txt; done
	@echo "Testing..." > test.txt
	@# Expecting a failure here, but reflect opposite result code
	@./aescrypt -e -p `cat test.passwd.txt` test.txt 2>/dev/null && \
	    echo Password length test failed && \
	    exit 1 || \
	    true
	@rm test.txt test.passwd.txt
	# Testing longer file
	@cat /dev/null >test.orig.txt
	@for i in `seq 1 50000`; do echo "This is a test" >>test.orig.txt; done
	@./aescrypt -e -p "praxis" test.orig.txt
	@cp test.orig.txt.aes test.txt.aes
	@./aescrypt -d -p "praxis" test.txt.aes
	@cmp test.orig.txt test.txt
	@rm test.orig.txt test.orig.txt.aes test.txt.aes test.txt
	@echo All file encryption tests passed
