NAME=barline

SRC=barline.c data.c

CC?=cc
CFLAGS+=-Wall -Wextra -Wpedantic -Werror -std=c99 -D_DEFAULT_SOURCE -DALSA
LDLIBS=-lasound

SRCDIR=src

PREFIX?=$(DESTDIR)/usr/local
BINDIR?=$(PREFIX)/bin
