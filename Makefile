DIRS= 01 02

.PHONY: subdirs $(DIRS) clean

subdirs: $(DIRS)

$(DIRS):
	$(MAKE) -C $@

clean: 
	rm -rf out/


