import os, sys, re
from subprocess import Popen, PIPE
from pprint import pprint

def replace_addresses(symboldata, profdata):
    pattern = re.compile(r'.*0x([\dabcdef]+)$')
    profdata = map(lambda x: x.strip(), profdata)

    for line in profdata:
        sys.stdout.write(line)
        match = pattern.match(line)
        if match:
            address = int(match.group(1), 16)
            print
            print "%d is in symboldata? %s" % (address, symboldata.has_key(address))
            if address in symboldata.keys():
                sys.stdout.write(" %s" % symboldata[address])
        sys.stdout.write('\n')

def get_pprof_output(filename, profilename):
    p = Popen(['/usr/local/bin/pprof'
              , '--text' 
              , filename 
              , profilename], stdout=PIPE)
    return p.stdout.readlines()

def get_symbols(filename):
    symboltable = {}
    pattern = re.compile(r'^([\dabcdef]+)\s+\w+\s+([\w_]+)')

    p = Popen(['/usr/bin/nm', filename], stdout=PIPE)
    for line in p.stdout.readlines():
        match = pattern.match(line)
        if match:
            symboltable[int(match.group(1), 16)] = match.group(2)

    return symboltable

if __name__ == "__main__":
    symbols = get_symbols('scheme')
    pprint(symbols)
    pprof_output = get_pprof_output('scheme', '/tmp/prof.out')
    replace_addresses(symbols, pprof_output)

