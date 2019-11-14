apps = hello_world simple_win

all: $(apps)

$(apps):
	$(MAKE) -C $@ clean all

.PHONY: $(apps)