all: arthm.c arthm.h
	gcc arthm.c -O3 -DSTANDALONE -lm -o eval

debug: arthm.c arthm.h
	gcc arthm.c -O3 -DDEBUG -DSTANDALONE -lm -o eval

test: tests/test.o arthm.c
	gcc arthm.c tests/test.c -lm -O3 -o test

clean:
	$(RM) arthm
	$(RM) test
