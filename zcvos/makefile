SUBDIRS = kernel build
BINDIR = bin

define make_subdir
	@for subdir in $(SUBDIRS) ; do \
	( cd $$subdir && make $1) \
	done;
endef

all:
	@cp -f lib/*.o build/
	$(call make_subdir , all)
install:
	$(call make_subdir , install)
test:
	$(call make_subdir , test)
clean:
	$(call make_subdir , clean)
	@rm -rf headers/*.h libzcvos.so
