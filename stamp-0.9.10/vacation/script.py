#!/usr/bin/env python

import subprocess
import numpy
import re
import sys

def timeout_command(cmd, timeout):
    """call shell-command and either return its output or kill it
    if it doesn't normally exit within timeout seconds and return None"""
    import datetime, os, time, signal

    start = datetime.datetime.now()
    process = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    while process.poll() is None:
        time.sleep(0.1)
        now = datetime.datetime.now()
        if (now - start).seconds > timeout:
            os.kill(process.pid, signal.SIGKILL)
            os.waitpid(-1, os.WNOHANG)
            return None

    return process.stdout.read()
def run_experiment(cmd, t):
    file.write(" ".join(cmd) + "\n")
    time = []
    for i in range(t):
        while (True) :
            out = timeout_command(cmd, 120)
            if not  out == None:
                break
            print "Timed out ", cmd
        out = re.search("(?<=Time = )[0-9]*\.[0-9]*", out).group(0)
        time.append(float(out))
        file.write(out + " ")
    file.write("\n")
    file.write("Std dev = " + "%.3f" % (numpy.std(time)))
    file.write("\n")
    file.write("min = " + "%.3f" % (numpy.amin(time)) + "\n")
    file.write("max = " + "%.3f" % (numpy.amax(time)) + "\n")
    return (numpy.median(time), numpy.amin(time), numpy.amax(time))

def run_benchmark(cmd, mode, nthreads):
    subprocess.check_output(['make', '-f', 'Makefile.seq', 'clean'], stderr=subprocess.STDOUT)
    subprocess.check_output(['make', '-f', 'Makefile.'+mode], stderr=subprocess.STDOUT)
    file.write(mode)
    file.write("\n")
    time_list = []
    min_list  = []
    max_list = []
    for n in nthreads:
        cmd[1] = '-c' + str(n)
        time, min, max = run_experiment(cmd,t)
        time_list.append(time)
        min_list.append(min)
        max_list.append(max)
        
    return (time_list, min_list, max_list)
       
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

def print_(precision, width, i, seq_time, tuples, types):
    def seq_string():
        string = "{"
        if seq_time:
            string += "'seq' : "
            string += "{0:{width}.{precision}}, ".format(seq_time[0], precision=precision, width=width)
        return string

    string = ""
    for elem in zip(*tuples):
        string += seq_string()
        for data, bench in zip(elem, types):
            string += "'%s' : " % bench
            string += "{0:{width}.{precision}}".format(data[i], precision=precision, width=width)
            if types.index(bench) != len(types)-1:
                string += ", "
        string += "},"
        string += "\n"

    return string


if __name__ == "__main__":
    if len(sys.argv) > 1 and sys.argv[1] == 'high':
        cmds = [['./vacation', '-c1', '-n8', '-q1', '-u60', '-r104857', '-t4194304']] #high
    else:
        #cmds = [['./vacation', '-c1', '-n2', '-q90', '-u98', '-r4194304', '-t16777216']]
        cmds = [['./vacation', '-c1', '-n2', '-q90', '-u98', '-r1048576', '-t4194304']] #low
    name = ""
    if len(sys.argv) > 2 and sys.argv[2] == 'boosting':
        name = "-boosting"
    types = ['seq', 'stm', 'STO', 'boosting'] if name == '-boosting' else ['seq', 'stm', 'STO', 'gen'] 
    file = open("tmp%s.txt" % name, 'w')
    out_file = open("results.txt", 'w')
    nthreads = [4, 16]
    t = 5;
    for cmd in cmds:
        out_file.write(" ".join(cmd) + "\n")
        out_file.write(printfmt_int('n', nthreads, 2, 10) + "\n")
        tuples = []
        seq_time = None
        for bench in types:
            threads = [1] if bench == 'seq' else nthreads
            time_, min_, max_ = run_benchmark(cmd, bench, threads)
            if bench == 'seq':
                seq_time = time_
            else:
                tuples.append((time_, min_, max_))
            out_file.write(printfmt_double(bench, time_, 4, 10) + "\n")
        out_file.write("[\n[\n")
        for i in xrange(len(nthreads)):
            out_file.write("[\n")
            out_file.write(print_(4, 8, i, seq_time, tuples, [bench for bench in types if bench != 'seq']))
            out_file.write("],\n")
        out_file.write("],\n]\n")
