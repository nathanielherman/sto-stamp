#!/usr/bin/env python

import subprocess
import numpy
import re

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
        while True:
            out = timeout_command(cmd, 700)
            g = re.search("(?<=Time = )[0-9]*\.[0-9]*", out)
            if g == None:
                print "retry " + str(cmd)
                continue
            out = g.group(0)
            time.append(float(out))
            file.write(out + " ")
            break
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
        cmd[1] = '-t' + str(n)
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

    print string
    return string

def printfmt_int(header, out, precision, width):
    string = ""
    string += "{0:{width}}".format(header, width=width)
    for a in out:
        string += "{0:{width}}".format(a, width=width)

    print string
    return string

def print_(precision, width, i, seq_time, stm_time, stm_min, stm_max, sto_time, sto_min, sto_max, gen_time, gen_min, gen_max):
    string = ""
    string += "{'seq' : "
    string += "{0:{width}.{precision}}".format(seq_time[0], precision=precision, width=width)
    string += ", 'sto' : "
    string += "{0:{width}.{precision}}".format(sto_time[i], precision=precision, width=width)
    string += ", 'stm' : "
    string += "{0:{width}.{precision}}".format(stm_time[i], precision=precision, width=width)
    string += ", 'gen' : "
    string += "{0:{width}.{precision}}".format(gen_time[i], precision=precision, width=width)
    string += "}"

    string += "\n"
    
    string += "{'seq' : "
    string += "{0:{width}.{precision}}".format(seq_time[0], precision=precision, width=width)
    string += ", 'sto' : "
    string += "{0:{width}.{precision}}".format(sto_min[i], precision=precision, width=width)
    string += ", 'stm' : "
    string += "{0:{width}.{precision}}".format(stm_min[i], precision=precision, width=width)
    string += ", 'gen' : "
    string += "{0:{width}.{precision}}".format(gen_min[i], precision=precision, width=width)
    string += "}"

    string += "\n"
    
    string += "{'seq' : "
    string += "{0:{width}.{precision}}".format(seq_time[0], precision=precision, width=width)
    string += ", 'sto' : "
    string += "{0:{width}.{precision}}".format(sto_max[i], precision=precision, width=width)
    string += ", 'stm' : "
    string += "{0:{width}.{precision}}".format(stm_max[i], precision=precision, width=width)
    string += ", 'gen' : "
    string += "{0:{width}.{precision}}".format(gen_max[i], precision=precision, width=width)
    string += "}"
    string += "\n"

    print string
    return string

if __name__ == "__main__":
    file = open("tmp.txt", 'w')
    out_file = open("results.txt", 'w')
    nthreads = [4,16]
    t = 5;
    cmds = [['./genome','-t1', '-g16384', '-s64', '-n16777216']]
    for cmd in cmds:
        out_file.write(" ".join(cmd) + "\n")
        out_file.write(printfmt_int('n', nthreads, 2, 8) + "\n")
        seq_time, seq_min, seq_max = run_benchmark(cmd, 'seq', [1])
        out_file.write(printfmt_double('seq', seq_time, 4, 8) + "\n")
        stm_time, stm_min, stm_max  = run_benchmark(cmd, 'stm', nthreads)
        out_file.write(printfmt_double('stm', stm_time, 4, 8) + "\n")
        sto_time, sto_min, sto_max  = run_benchmark(cmd, 'STO', nthreads)
        out_file.write(printfmt_double('sto', sto_time, 4, 8) + "\n")
	gen_time, gen_min, gen_max  = run_benchmark(cmd, 'gen', nthreads)
	out_file.write(printfmt_double('genSTM', gen_time, 4, 8) + "\n")
        out_file.write(print_(4, 8, 0, seq_time, stm_time, stm_min, stm_max, sto_time, sto_min, sto_max, gen_time, gen_min, gen_max))
	out_file.write(print_(4, 8, 1, seq_time, stm_time, stm_min, stm_max, sto_time, sto_min, sto_max, gen_time, gen_min, gen_max))
