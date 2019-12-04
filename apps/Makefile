apps = simple_win demo1 desktop

$(apps):
	$(MAKE) -C $@ $(TARGET)

all: 
	$(foreach app, $(apps), $(MAKE) -C $(app) $@;)	

clean: 
	$(foreach app, $(apps), $(MAKE) -C $(app) $@;)	

install: 
	$(foreach app, $(apps), $(MAKE) -C $(app) $@;)	

.PHONY: $(apps) all clean install