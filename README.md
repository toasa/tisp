# tisp

Lisp interpreter written in C.

## DEMO

By REPL, compute the 10th term of the Fibonacci sequence:

```bash
$ make run
tisp> (defun fibo (n) (if (< n 3) 1 (+ (fibo (- n 1)) (fibo (- n 2)))))         
fibo
tisp> (fibo 10)
55
```

## REPL

You can run REPL by running

```
$ make run
```

on top of the source tree.

## Test

You can do all unittests by running 

```
$ make test
```

on top of the source tree.
The unittests are defined in `test/main.py`.

## Requirement

- Python3 (for test)
