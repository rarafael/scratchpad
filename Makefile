include config.mk

BINDIR=bin
SRCDIR=src

.PHONY: all clean
all: $(BINDIR)\
     $(BINDIR)/bconv\
     $(BINDIR)/distroget

install: all
	$(INSTALL) -D $(BINDIR)/bconv $(BINPREFIX)/
	$(INSTALL) -D $(BINDIR)/distroget $(BINPREFIX)/

uninstall: $(BINPREFIX)/bconv\
           $(BINPREFIX)/distroget
	rm -i $^

clean:
	rm $(BINDIR)/*
	rmdir $(BINDIR)

$(BINDIR):
	mkdir bin

$(BINDIR)/bconv: $(SRCDIR)/bconv.c
	$(CC) $(CFLAGS) $< -o $@
$(BINDIR)/distroget: $(SRCDIR)/distroget.c
	$(CC) $(CFLAGS) $< -o $@
