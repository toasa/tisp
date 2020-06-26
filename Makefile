run:
	clang -o main main.c cell.c token.c util.c
	./main

clean:
	rm main
