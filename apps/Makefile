apps = countdown hello_world

all: $(apps)

$(apps):
	$(MAKE) -C $@ clean all

.PHONY: $(apps)