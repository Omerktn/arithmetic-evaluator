all: arthm.c arthm.h
	gcc arthm.c -ffloat-store -O3 -DSTANDALONE -lm -o eval

debug: arthm.c arthm.h
	gcc arthm.c -ffloat-store -g -DDEBUG -DSTANDALONE -lm -o eval

test: tests/test.o arthm.c
	gcc arthm.c -ffloat-store tests/test.c -lm -O3 -o test

clean:
	$(RM) arthm
	$(RM) test
