DIRS= 01 02 03 04 05 06 07

.PHONY: subdirs $(DIRS) clean

subdirs: $(DIRS)

$(DIRS):
	$(MAKE) -C $@

clean: 
	rm -rf out/


