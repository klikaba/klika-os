apps = simple_win desktop

all: $(apps)

$(apps):
	$(MAKE) -C $@ clean all install

.PHONY: $(apps)