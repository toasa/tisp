compile:
	clang -o main main.c cell.c token.c util.c

run: compile
	./main

test: compile
	python3 test/main.py

clean:
	rm main test/tmp.txt
