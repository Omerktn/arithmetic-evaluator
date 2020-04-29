all: arthm.c
	gcc arthm.c -O3 -lm -o eval

clean:
	$(RM) arthm
