BUILDDIR = ../build
CONF = -DDEBUG -O0 -g3 -Wall -c -fmessage-length=0 -std=c99 -D_XOPEN_SOURCE=600

all:
	@for obj in `ls *.c` ; do \
	( $(CC) $(CONF) $$obj )\
	done;
	@cp *.o ../build;
clean:
	@rm -rf *.o *.so; 
