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
                    ["(cond ((eq 1 2) 1))", "NIL\n"],
                    ["(cond (1 1) (2 2))", "1\n"],
                ]
        self.do_tests(tests)

    def test_append(self):
        tests = [
                    ["(append '(1 2) '(3 4))", "(1 2 3 4)\n"],
                    # TODO: print bug
                    # ["(append '(1 2) 3)", "(1 2 . 3)\n"],
                    # ["(append '(1 2 3) 'foo)", "(1 2 3 . foo)\n"],
                ]
        self.do_tests(tests)

    def test_defun(self):
        tests = [
                    ["(defun add (x y) (+ x y))", "add\n"],
                    ["(defun add (x y) (+ x y))(add 10 20)", "30\n"],
                    ["(defun add (x y) (+ x y))(add 10 20)(defun addThree (x y z) (+ x y z))(addThree 1 3 5)", "9\n"],
                    ["(defun retThree () 3)(retThree)", "3\n"],
                    ["(defun add (x y) (+ x y))(add (+ 10 10) 20)", "40\n"],
                    ["(defun add (x y) (+ x y))(defun addThree (x y z) (+ x y z))(addThree (add 1 2) (add 3 4) (add 5 6))", "21\n"],
                    ["(defun bool (b) (if b 10 20))(bool T)", "10\n"],
                    ["(defun bool (b) (if b 10 20))(bool NIL)", "20\n"],
                    ["(setq x 10)(defun f (x) (+ x 1))(f 10)x", "10\n"],
                    ["(setq x 10)(defun f (x) (+ x 1))x(f 10)", "11\n"],
                    ["(setq x 10)(defun inc () (setq x (+ x 1)))(inc)(inc)(inc)x", "13\n"],
                    ["(defun fibo (n) (if (< n 3) 1 (+ (fibo (- n 1)) (fibo (- n 2)))))(fibo 10)", "55\n"],
                ]
        self.do_tests(tests)

    def test_length(self):
        tests = [
                    ["(length '(10 20 30))", "3\n"],
                    ["(length '())", "0\n"],
                    ["(length ''())", "2\n"],
                ]
        self.do_tests(tests)

    def test_if(self):
        tests = [
                    ["(if T 3 4)", "3\n"],
                    ["(if NIL 3 4)", "4\n"],
                    ["(if (eq (+ 1 2) 3) 'true 'false)", "true\n"],
                    ["(if (eq (+ 1 2) 5) 'true 'false)", "false\n"],
                    ["(if T 3)", "3\n"],
                    ["(if NIL 3)", "NIL\n"],
                ]
        self.do_tests(tests)

    def test_list(self):
        tests = [
                    ["(list 2 3 4)", "(2 3 4)\n"],
                    ["(list (+ 2 3) 4)", "(5 4)\n"],
                    ["(list 2 3 4 NIL '(1 2 3))", "(2 3 4 NIL (1 2 3))\n"],
                    ["(list)", "NIL\n"],
                ]
        self.do_tests(tests)

    def test_setq(self):
        tests = [
                    ["(setq x 5)", "5\n"],
                    ["(setq x 5) x", "5\n"],
                    ["(setq x T) x", "T\n"],
                    ["(setq x (+ 10 20)) x", "30\n"],
                    ["(setq x 1)(defun inc () (+ x 1))(inc)", "2\n"],
                ]
        self.do_tests(tests)

    def test_add(self):
        tests = [
                    ["(+ 2)", "2\n"],
                    ["(+ 2 2)", "4\n"],
                    ["(+ 2 2 2 2 2)", "10\n"],
                    ["(+ (+ 20 30) (+ 30 40))", "120\n"],
                    ["(+ '1 7)", "8\n"],
                ]
        self.do_tests(tests)

    def test_sub(self):
        tests = [
                    ["(- 10)", "-10\n"],
                    ["(- 10 20 30 60)", "-100\n"],
                    ["(- (+ 10 20) (- 10 40))", "60\n"],
                ]
        self.do_tests(tests)

    def test_mul(self):
        tests = [
                    ["(* 3 5)", "15\n"],
                    ["(+ (* 2 10 10 10) 20)", "2020\n"],
                ]
        self.do_tests(tests)

    def test_lt(self):
        tests = [
                    ["(< 1 2)", "T\n"],
                    ["(< 2 1)", "NIL\n"],
                    ["(> 1 2)", "NIL\n"],
                    ["(> 2 1)", "T\n"],
                    ["(< (+ 20 30) (+ 30 40))", "T\n"],
                ]
        self.do_tests(tests)

if __name__ == "__main__":
    unittest.main()
