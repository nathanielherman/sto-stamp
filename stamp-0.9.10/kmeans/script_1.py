#!/usr/bin/env python

import subprocess
import numpy
import re

def run_experiment(cmd, t):
    for i in range(t):
        time = []
        out = subprocess.check_output(cmd)
        out = re.search("(?<=Time: )[0-9]*\.[0-9]*", out).group(0)
        time.append(float(out))
    return (numpy.mean(time), numpy.std(time))

def run_benchmark(cmd, mode, nthreads):
    if mode == 'stm':
        cmd[0] = './kmeans-tl2'
    else:
        subprocess.check_output(['make', '-f', 'Makefile.seq', 'clean'], stderr=subprocess.STDOUT)
        subprocess.check_output(['make', '-f', 'Makefile.'+mode], stderr=subprocess.STDOUT)
   
    time_list = []
    std_list  = []
    for n in nthreads:
        cmd[1] = '-p' + str(n)
        time, std = run_experiment(cmd,t)
        time_list.append(time)
        std_list.append(std)

    cmd[0] = './kmeans'
    return (time_list, std_list)
       
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
    nthreads = [1,2,4,8,16,32]
    t = 1;
    cmds = [['./kmeans', '-p1', '-m80', '-n80', '-t0.001', '-i', 'inputs/random-n65536-d32-c16.txt'], \
            ['./kmeans', '-p1', '-m160', '-n160', '-t0.001', '-i', 'inputs/random-n65536-d32-c16.txt']]

    for cmd in cmds:
        print cmd
        print printfmt_int('n', nthreads, 4, 8)
        seq_time, seq_std = run_benchmark(cmd, 'seq', [1])
        print printfmt_double('seq', seq_time, 4, 8)
        stm_time, stm_std = run_benchmark(cmd, 'stm', nthreads)
        print printfmt_double('stm', stm_time, 4, 8)
        sto_time, sto_std = run_benchmark(cmd, 'STO', nthreads)
        print printfmt_double('sto', sto_time, 4, 8)

