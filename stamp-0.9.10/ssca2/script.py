#!/usr/bin/env python

import subprocess
import numpy
import re

def run_experiment(cmd, t):
    file.write(" ".join(cmd) + "\n")
    time = []
    for i in range(t):
        out = subprocess.check_output(cmd)
        out = re.search("(?<=Time =  )[0-9]*\.[0-9]*", out).group(0)
        time.append(float(out))
        file.write(out + " ")
    file.write("\n")
    file.write("Std dev = " + "%.3f" % (numpy.std(time)))
    file.write("\n")
    return (numpy.median(time))

def run_benchmark(cmd, mode, nthreads):
    subprocess.check_output(['make', '-f', 'Makefile.seq', 'clean'], stderr=subprocess.STDOUT)
    subprocess.check_output(['make', '-f', 'Makefile.'+mode], stderr=subprocess.STDOUT)
    file.write(mode)
    file.write("\n")
    time_list = []
    std_list  = []
    for n in nthreads:
        cmd[1] = '-t' + str(n)
        time = run_experiment(cmd,t)
        time_list.append(time)
        
    return (time_list)
       
def printfmt_double(header, out, precision, width):
    string = ""
    string += "{0:{width}}".format(header, width=width)
    for a in out:
        string += "{0:{width}.{precision}}".format(a, precision=precision, width=width)
    return string

def printfmt_int(header, out, precision, width):
    string = ""
    string += "{0:{width}}".format(header, width=width)
    for a in out:
        string += "{0:{width}}".format(a, width=width)
    return string

if __name__ == "__main__":
    file = open("tmp.txt", 'w')
    out_file = open("results.txt", 'w')
    nthreads = [1,2,4,8,16]
    t = 1;
    cmds = [['./ssca2','-t1', '-s21', '-i1.0', '-u1.0', '-l3', '-p3']]
    for cmd in cmds:
        out_file.write(" ".join(cmd) + "\n")
        out_file.write(printfmt_int('n', nthreads, 2, 8) + "\n")
        seq_time = run_benchmark(cmd, 'seq', [1])
        out_file.write(printfmt_double('seq', seq_time, 4, 8) + "\n")
        stm_time = run_benchmark(cmd, 'stm', nthreads)
        out_file.write(printfmt_double('stm', stm_time, 4, 8) + "\n")
	gen_time = run_benchmark(cmd, 'gen', nthreads)
	out_file.write(printfmt_double('genSTM', gen_time, 4, 8) + "\n")