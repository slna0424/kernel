CC = gcc
CFLAGS = -shared -fpic -lpthread

all:
	@cp ../lib/*.o . ;
	$(CC) -o libzcvos.so `ls *.o`  $(CFLAGS);
	ar rcs libzcvos_s.a *.o
install:
	@cp libzcvos.so libzcvos_s.a ../
clean:
	@rm -f *.so *.o ; \
