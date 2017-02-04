CC = gcc
CFLAGS = -lpthread -lzcvos -L.

all: 
	@for case in `ls test_*.o | cut -d . -f 1` ; do \
	( $(CC) -o $$case $$case.o $(CFLAGS)) ; \
	chmod a+x $$case ; \
	cp $$case ../ ; \
	done;	
clean:
	@rm -f *.so *.o test_*;
