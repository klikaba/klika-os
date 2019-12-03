apps = apps kernel

all: 
	$(MAKE) -C apps clean all install
	$(MAKE) -C kernel clean all install

run: all
	$(MAKE) -C kernel run

.PHONY: $(apps) all 

