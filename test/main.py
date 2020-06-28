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

    def test_list(self):
        tests = [ 
                    ["(quote (2))", "(2)\n"],
                    ["(quote (10 20 30))  ", "(10 20 30)\n"],
                    ["(quote (10 20 (30 40) 50))", "(10 20 (30 40) 50)\n"],
                    ["(quote (((10))))", "(((10)))\n"],
                    ["(quote (1 2 ((3)) (4 (5 6))))", "(1 2 ((3)) (4 (5 6)))\n"],
                    ["(quote (quote 2))", "(2)\n"],
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
                    ["(eq (quote (10 20 30)) (quote (10 20 30)))", "NIL\n"],
                    ["(eq (quote (10 20 30)) (quote (11 22 33)))", "NIL\n"],
                ]
        self.do_tests(tests)

if __name__ == "__main__":
    unittest.main()
