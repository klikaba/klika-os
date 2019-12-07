apps = apps kernel

all: 
	$(MAKE) -C apps clean all install assets
	$(MAKE) -C kernel clean all install
ci:
	$(MAKE) -C apps clean all
	$(MAKE) -C kernel clean all 
run: all
	$(MAKE) -C kernel run

.PHONY: $(apps) all 

