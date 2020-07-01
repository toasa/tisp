import unittest
import os
from test import support

class TestRun(unittest.TestCase):
    OUTPUT_PATH = "test/tmp.txt"

    def exe(self, arg):
        os.system("./main \"{0}\" > {1}".format(arg, self.OUTPUT_PATH))

    def do_tests(self, tests):
        if not os.path.exists(self.OUTPUT_PATH):
            os.system("touch {0}".format(self.OUTPUT_PATH))

        f = open(self.OUTPUT_PATH)

        for test in tests:
            input_ = test[0]
            expected = test[1]

            self.exe(input_);

            actual = f.read()
            self.assertEqual(actual, expected);
            f.seek(0, 0)

        f.close()

    def test_num(self):
        tests = [ 
                    ["2", "2\n"],
                    ["30  ", "30\n"],
                    ["  120", "120\n"],
                    [" 5   ", "5\n"],
                    ["(quote 5)", "5\n"],
                ]
        self.do_tests(tests)

    def test_quote(self):
        tests = [
                    ["'2", "2\n"],
                    ["''30", "'30\n"],
                    ["'T", "T\n"],
                    ["'''NIL", "''NIL\n"],
                    ["'(10 20 30)", "(10 20 30)\n"],
                    ["'(foo bar baz)", "(foo bar baz)\n"],
                    ["'(+ 1 2)", "(+ 1 2)\n"],
                    ["(quote (car cdr cons))", "(car cdr cons)\n"],
                ]
        self.do_tests(tests)

    def test_list(self):
        tests = [ 
                    ["(quote (2))", "(2)\n"],
                    ["(quote (10 20 30))  ", "(10 20 30)\n"],
                    ["(quote (10 20 (30 40) 50))", "(10 20 (30 40) 50)\n"],
                    ["(quote ((10 20) 30 40))", "((10 20) 30 40)\n"],
                    ["(quote ((10)))", "((10))\n"],
                    ["(quote (((10))))", "(((10)))\n"],
                    ["(quote (1 2 ((3)) (4 (5 6))))", "(1 2 ((3)) (4 (5 6)))\n"],
                    ["(quote (quote 2))", "'2\n"],
                    ["(quote ())", "NIL\n"],
                ]
        self.do_tests(tests)

    def test_T_NIL(self):
        tests = [
                    ["T", "T\n"],
                    ["NIL  ", "NIL\n"],
                    ["(quote (T NIL T))", "(T NIL T)\n"],
                ]
        self.do_tests(tests)

    def test_eq(self):
        tests = [
                    ["(eq 10 10)", "T\n"],
                    ["(eq 10 15)", "NIL\n"],
                    ["(eq T T)", "T\n"],
                    ["(eq T NIL)", "NIL\n"],
                    ["(eq NIL NIL)", "T\n"],
                    ["(eq () ())", "T\n"],
                    ["(eq (quote (10 20 30)) (quote (10 20 30)))", "NIL\n"],
                    ["(eq (quote (10 20 30)) (quote (11 22 33)))", "NIL\n"],
                ]
        self.do_tests(tests)

    def test_atom(self):
        tests = [
                    ["(atom 10)", "T\n"],
                    ["(atom T)", "T\n"],
                    ["(atom NIL)", "T\n"],
                    ["(atom ())", "T\n"],
                    ["(atom (quote ()))", "T\n"],
                    ["(atom (quote (T NIL)))", "NIL\n"],
                    ["(atom (quote (1 2 3)))", "NIL\n"],
                    ["(atom (eq 1 2))", "T\n"],
                    ["(atom (eq 2 2))", "T\n"],
                    ["(atom (eq 2 (quote (30 30))))", "T\n"],
                    ["(atom (atom 10))", "T\n"],
                    ["(atom (quote (atom 10)))", "NIL\n"],
                ]
        self.do_tests(tests)

    def test_car(self):
        tests = [
                    ["(car (quote (10)))", "10\n"],
                    ["(car (quote (10 20)))", "10\n"],
                    ["(car (quote (T 10 NIL 20)))", "T\n"],
                ]
        self.do_tests(tests)

    def test_cdr(self):
        tests = [
                    ["(cdr (quote (10)))", "NIL\n"],
                    ["(cdr (quote (10 20)))", "(20)\n"],
                    ["(cdr (quote (T 10 NIL 20)))", "(10 NIL 20)\n"],
                ]
        self.do_tests(tests)

    def test_cons(self):
        tests = [
                    ["(cons 1 ())", "(1)\n"],
                    ["(cons 1 NIL)", "(1)\n"],
                    ["(cons 1 (quote (2 3)))", "(1 2 3)\n"],
                    ["(cons (quote (10 20)) (quote (30 40)))", "((10 20) 30 40)\n"],
                    ["(cons (car (quote (10 20 30))) (cdr (quote (10 20 30))))", "(10 20 30)\n"],
                    ["(cons 10 20)", "(10 . 20)\n"],
                    ["(cons (quote (10 20)) 30)", "((10 20) . 30)\n"],
                    ["(cons (quote (10 20)) NIL)", "((10 20))\n"],
                    ["(cons 1 (cons 2 (cons 3 ())))", "(1 2 3)\n"],
                    ["(car (cons 1 (quote (2 3))))", "1\n"],
                    ["(cdr (cons 1 (quote (2 3))))", "(2 3)\n"],
                    ["(cons (quote (10 20)) 30)", "((10 20) . 30)\n"],
                ]
        self.do_tests(tests)

    def test_cond(self):
        tests = [
                    ["(cond ((eq 1 1) 1) ((atom ()) 2))", "1\n"],
                    ["(cond ((eq 1 2) 1) ((atom ()) 2))", "2\n"],
                    ["(cond ((eq 1 2) 1) ((atom (quote (10 20)) 2))", "NIL\n"],
                    ["(cond (1 1) (2 2))", "1\n"],
                ]

    def test_add(self):
        tests = [
                    ["(+ 2)", "2\n"],
                    ["(+ 2 2)", "4\n"],
                    ["(+ 2 2 2 2 2)", "10\n"],
                    ["(+ (+ 20 30) (+ 30 40))", "120\n"],
                    ["(+ '1 7)", "8\n"],
                ]
        self.do_tests(tests)

if __name__ == "__main__":
    unittest.main()
