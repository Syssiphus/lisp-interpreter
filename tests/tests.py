#!/usr/bin/env python

import sys, os, subprocess

# Test strings
not_test = "(assert (not #f) #t)"

testlist = {
        # number predicates
        "eqv? 1" : "(assert-true (eqv? 'a 'a))"
      , "eqv? 2" : "(assert-false (eqv? 'a 'b))"
      , "eqv? 3" : "(assert-true (eqv? 2 2))"
      , "eqv? 4" : "(assert-true (eqv? '() '()))"
      , "eqv? 5" : "(assert-true (eqv? 100000000 100000000))"
      , "eqv? 6" : "(assert-false (eqv? (cons 1 2) (cons 1 2)))"
      , "eqv? 7" : "(assert-false (eqv? (lambda () 1) (lambda () 2)))"
      , "eqv? 8" : "(assert-false (eqv? #f 'nil))"
      , "eqv? 9" : "(assert-true (let ((p (lambda (x) x))) (eqv? p p)))"
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
    retval = "(begin %s (quit))" % test
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

        sys.stdout.flush()

        os.remove(result_file)
        os.remove(error_file)
        

    print "\nTotal %d tests (OK: %d, Failed: %d)" \
            % (len(tests), ok_count, fail_count)

if __name__ == "__main__":
    execute_testlist(testlist)

