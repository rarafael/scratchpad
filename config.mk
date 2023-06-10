# Compiler and command options
CC=cc
CFLAGS=-Wall -Wextra -std=c99 -Wpedantic -O3 -ggdb
INSTALL=install

# Install locations
PREFIX=/usr/local
BINPREFIX=$(PREFIX)/bin
MANPREFIX=$(PREFIX)/man
LIBPREFIX=$(PREFIX)/lib
INCPREFIX=$(PREFIX)/include
