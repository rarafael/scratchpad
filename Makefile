include config.mk

all:
	@$(MAKE) -C bconv $@
clean:
	@$(MAKE) -C bconv $@
install:
	@$(MAKE) -C bconv $@
