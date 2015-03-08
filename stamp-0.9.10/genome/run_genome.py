#!/usr/bin/env python

import os
import re
import sys
import json
import numpy
import subprocess
import multiprocessing

nthreads_max = 4

def makefile_name(mode):
	m = "Makefile"
	if mode == "STO": 
		m += ".STO"
	elif mode == "TL2":
		m += ".stm"
	elif mode == "SEQ":
		m += ".seq"
	return m

def build(mode):
	if os.path.exists("genome"):
		ret = os.system("make -f %s clean > /dev/null 2>&1" % makefile_name(mode))
		assert ret == 0

	print "Building genome with %s..." % mode
	makefile = makefile_name(mode)

	ret = os.system("make -f %s > /dev/null 2>&1" % makefile_name(mode))
	if ret != 0:
		print "FATAL: Build error!"
		sys.exit(1)
	else:
		print "Built."

def clean_up(mode):
	print "Cleanning up %s..." % mode
	ret = os.system("make -f %s clean > /dev/null 2>&1" % makefile_name(mode))
	assert ret == 0
	print "Done."

def attach_args(genelen, nsegments, seglen, nthreads):
	args = ["./genome", "-g%d" % genelen, "-n%d" % nsegments, "-s%d" % seglen, "-t%d" %nthreads]
	return args

def extract_time(output):
	time = re.search("(?<=Time = )[0-9]*\.[0-9]*", output).group(0)
	return float(time)

def run_series(trail, mode, records):
	assert mode == "STO" or mode == "TL2" or mode == "SEQ"
	assert trail >= 0

	genelen = 16384
	nsegments = 4194304
	seglen = 64
	nthreads = 1
	out_original = "@@@Running %s trail #%d" % (mode, trail)
	print out_original
	out_original += "\n"
	
	records[mode][trail] = dict()
	
	while nthreads <= nthreads_max:
		# set up and echo parameters
		args = attach_args(genelen, nsegments, seglen, nthreads)
		print args
		out_original += str(args)

		# run benchmark
		out = subprocess.check_output(args, stderr=subprocess.STDOUT)
		
		# register results
		out_original += out
		time = extract_time(out)
		records[mode][trail][nthreads] = time

		# don't increase nthreads for sequential run
		if mode == "SEQ":
			break

		nthreads = nthreads * 2
		nsegments = nsegments * 2

	return out_original

def run_all(rounds, modes):
	print "Running genome for %d rounds for each implementation..." % rounds
	records = dict()
	for mode in modes:
		records[mode] = dict()
		build(mode)
		mode_stdout = ""

		for trail in range(0, rounds):
			mode_stdout += run_series(trail, mode, records)
		f = open("./results/" + mode + "_stdout.txt", "w")
		f.write(mode_stdout)
		f.close()
		clean_up(mode)

	f = open("./results/records.json", "w")
	f.write(json.dumps(records))
	f.close()

	return records

def process_records(records, modes, ntrails):
	summary = dict()
	for mode in modes:
		summary[mode] = dict()

		t = 1
		while t <= nthreads_max:
			runtimes = []
			for n in range(0, ntrails):
				runtimes.append(records[mode][n][t])
			mean = numpy.mean(runtimes)
			stddev = numpy.std(runtimes)
			summary[mode][t] = [mean, stddev]

			# doing the same thing to skip higher numbers of
			# threads for the sequential version
			if mode == "SEQ":
				break;

			t *= 2

	f = open("./results/summary.json", "w")
	f.write(json.dumps(summary))
	f.close()

	print "All trails finished, summary saved in \"results/summary.json\""

	return summary

def print_summary(summary):
	print "Running time summary:"
	for mode, rt_info in summary.iteritems():
		print "genome run times with %s:" % mode
		for nthreads, result_pair in sorted(rt_info.iteritems()):
			print "\tthreads: %d\tmean: %f\tstddev: %f" % (nthreads, result_pair[0], result_pair[1])

def main(argc, argv):
	if argc != 2:
		print "Usage: %s [num_trails]" % argv[0]
		sys.exit(0)
	elif int(argv[1]) > 10 or int(argv[1]) <= 0:
		print "Please specify num_trails within integer range [1, 10]"
		sys.exit(0)

	modes = ["SEQ", "TL2", "STO"]
	ntrails = int(argv[1])

	records = run_all(ntrails, modes)
	summary = process_records(records, modes, ntrails)
	print_summary(summary)

if __name__ == "__main__":
	nthreads_max = multiprocessing.cpu_count()
	main(len(sys.argv), sys.argv)