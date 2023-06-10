include ../config.mk

BINARY=bconv

$(BINARY): $(BINARY).c
	$(CC) $(CFLAGS) $< -o $@

all: $(BINARY)

install: $(BINARY)
	$(INSTALL) -D $< $(BINPREFIX)/$<

clean:
	rm $(BINARY)
