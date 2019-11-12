SUBDIRS = projectX projectY

all: kernel apps

kernel:
	make -C kernel clean all
apps:
	make -C apps 
run: $(kernel) $(apps)
	make -C kernel clean all run

.PHONY: kernel apps