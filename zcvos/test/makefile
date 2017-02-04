SUBDIRS = kernel build

define make_subdir
	@for subdir in $(SUBDIRS) ; do \
	( cd $$subdir && make $1) \
	done;
endef

all:
	@cp ../build/libzcvos.so build
	$(call make_subdir , all)
clean:
	$(call make_subdir , clean)
	@rm -rf ./test_* *.so
