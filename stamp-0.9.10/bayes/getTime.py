import sys
import re
import math

class ExtractTime:
    def __init__(self, file, filename):
        self.lines = []
        self.filename = filename
        self.i = 0
        for x in file:
            self.process(x)
        
    def patternMatch(self, line, pattern):
        p = re.compile(pattern);
        m = p.search(line)
        if m != None:
            print m.group().split()[3]
    def process(self, line):
        self.patternMatch(line, "Learn time = .*")

def main(argv):
    filename = argv[0]
    f = open(filename, 'r')
    e = ExtractTime(f, filename)
    f.close()

if (__name__=='__main__'):
    main(sys.argv[1:])
