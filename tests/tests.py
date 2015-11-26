#!/usr/bin/env python

import sys, os, subprocess

# Test strings
not_test = "(assert (not #f) #t)"

testlist = {
      # The little schemer chapter 1
      # atoms
      "tls_atom1" : "(assert-true (atom? 'atom))"
    , "tls_atom2" : "(assert-true (atom? 'turkey))"
    , "tls_atom3" : "(assert-true (atom? 1492))"
    , "tls_atom4" : "(assert-true (atom? 'u))"
    , "tls_atom5" : "(assert-true (atom? '*abc$))"

      #lists
    , "tls_list1" : "(assert-true (pair? '(atom)))"
    , "tls_list2" : "(assert-true (pair? '(atom turkey or)))"
    , "tls_list3" : "(assert (length '(((how) are) ((you) (doing so)) far)) 3)"
    , "tls_list4" : "(assert-true (pair? '()))"
    , "tls_list5" : "(assert-false (atom? '()))"
    , "tls_list6" : "(assert-true (pair? '(() () () ())))"

      # car
    , "tls_car1"  : "(assert (car '(a b c)) 'a)"
    , "tls_car2"  : "(assert (car '((a b c) x y z)) '(a b c))"
    }

interpreter = "%s/../scheme" % os.path.dirname(sys.argv[0])
result_file = "%s/result.txt" % os.path.dirname(sys.argv[0])
error_file  = "%s/error.txt" % os.path.dirname(sys.argv[0])

def escape_test_string(s):
    retval = ""
    for c in s:
        if c == "'":
            retval = "%s\\'" % retval
        else:
            retval = "%s%s" % (retval, c)
    return retval

def wrap_test(test):
    retval = "(begin (load \"src/stdlib.scm\") %s (quit))" % test
    return retval

def interpreter_cmd():
    return "%s > %s > %s" % (interpreter, result_file, error_file)

def create_testrun_cmd(test):
    global interpreter, result_file, error_file
    return "echo \'%s\' | %s" % (test, interpreter_cmd())

def execute_testlist(tests):
    ok_count = 0
    fail_count = 0

    for testname in sorted(tests.keys()):
        test = wrap_test(tests[testname])

        sys.stdout.write("Test '%s': %s" % (testname, tests[testname]))

        stdout_file = open(result_file, "w")
        stderr_file = open(error_file, "w")

        proc = subprocess.Popen([interpreter], 
                stdin=subprocess.PIPE, 
                stdout=stdout_file,
                stderr=stderr_file,
                shell=True)

        proc.stdin.write(test)
        proc.wait()
        result = proc.returncode

        stdout_file.close()
        stderr_file.close()

        if result != 0:
            # test failed
            sys.stdout.write(" FAILED.\n")
            fail_count += 1

            f = open(error_file, "r")
            sys.stdout.write("Reason:\n%s\n" % f.read())
            f.close()
        else:
            # test good
            sys.stdout.write(" OK.\n")
            ok_count += 1

        os.remove(result_file)
        os.remove(error_file)
        

    print "\nTotal %d tests (OK: %d, Failed: %d)" \
            % (len(tests), ok_count, fail_count)

if __name__ == "__main__":
    execute_testlist(testlist)

