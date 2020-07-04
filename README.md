# tisp

Lisp interpreter written in C.
The tisp supports following primitives:

- `quote`
- `atom`
- `eq`
- `car`
- `cdr`
- `cons`
- `cond`
- `list`
- `append`
- `defun`
- `setq`
- `if`
- `length`

And also supports the following arithmetic operators:

- `+`
- `-`
- `*`
- `/`
- `mod`
- `>`
- `<`

## Run REPL

```
$ make run
```

## Test

You can do unittests defined in `test/main.py`.
If you want to do, run the following command.

```
$ make test
```

## Requirement

- Python3 (for test)
