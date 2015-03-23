#!/usr/bin/env python

import subprocess
import re
import os
def run_tl2(bench, config):
  os.chdir('./' + bench)
  subprocess.check_output(['make', '-f', 'Makefile.stm', 'clean'], stderr=subprocess.STDOUT)
  subprocess.check_output(['make', '-f', 'Makefile.stm'], stderr=subprocess.STDOUT)
  cmd = ['./' + bench]
  cmd.extend(config.split())
  out = subprocess.check_output(cmd)
  #print out
  #print re.search("(?<=Starts=)[0-9]*", out)
  starts = re.search("(?<=Starts=)[0-9]*", out).group(0)
  reads = re.search("(?<=0: )[0-9]*", out).group(0)
  writes = re.search("(?<=1: )[0-9]*", out).group(0)
  search = re.search("(?<=2: )[0-9]*", out).group(0)
  check_reads = re.search("(?<=3: )[0-9]*", out).group(0)
  wrset_total = re.search("(?<=4: )[0-9]*", out).group(0)
  wrset_max = re.search("(?<=5: )[0-9]*", out).group(0)
  rset_total = re.search("(?<=6: )[0-9]*", out).group(0)
  rset_max = re.search("(?<=7: )[0-9]*", out).group(0)
  wrset_avg = str(int(wrset_total) / int(starts))
  rset_avg = str(int(rset_total)/int(starts))

  stats = [starts, \
            reads, \
            writes, \
            search, \
            check_reads, \
            wrset_avg, \
            wrset_max, \
            rset_avg, \
            rset_max ]
  os.chdir('..')
  return stats


def run_sto(bench, config, ver):
  os.chdir('./' + bench)
  subprocess.check_output(['make', '-f', 'Makefile.seq', 'clean'], stderr=subprocess.STDOUT)
  subprocess.check_output(['make', '-f', 'Makefile.' + ver], stderr=subprocess.STDOUT)
  cmd = ['./' + bench]
  cmd.extend(config.split())
  out = subprocess.check_output(cmd)
  #print out
  starts = re.search("(?<=starts: )[0-9]*", out).group(0)
  reads = re.search("(?<=read: )[0-9]*", out).group(0)
  writes = re.search("(?<=write: )[0-9]*", out).group(0)
  search = re.search("(?<=searched: )[0-9]*", out).group(0)
  check_reads = re.search("(?<=check_read: )[0-9]*", out).group(0)
  set_avg = re.search("(?<=average set size: )[0-9]*", out).group(0)
  set_max = re.search("(?<=max set size: )[0-9]*", out).group(0)
  
  stats = [starts, \
    reads, \
    writes, \
    search, \
    check_reads, \
    set_avg, \
    set_max ]
  os.chdir('..')
  return stats



def printfmt_int(bench, type, out, width):
  string = ""
  string += "{0:{width}}".format(bench, width=width)
  string += "{0:{width}}".format(type, width=width)

  for a in out:
    string += "{0:{width}}".format(a, width=width)
  return string

if __name__ == "__main__":
  print "Make sure to run this with detailed logging enabled in both STO and tl2"
  width = 15
  configs = {'bayes' : '-v32 -r4096 -n10 -p40 -i2 -e8 -s1', \
  'genome' : '-g16384 -s64 -n16777216', \
  'intruder' : '-a10 -l128 -n262144 -s1', \
  'kmeans' : '-m40 -n40 -t0.00001 -i inputs/random-n65536-d32-c16.txt', \
  'labyrinth' : '-i inputs/random-x512-y512-z7-n512.txt', \
  'ssca2' : '-s20 -i1.0 -u1.0 -l3 -p3', \
  'vacation' : '-n2 -q90 -u98 -r1048576 -t4194304', \
  'yada' : '-a15 -i inputs/ttimeu1000000.2' }
  
  
  bench = ['bayes', 'genome', 'intruder', 'kmeans', 'vacation', 'labyrinth', 'ssca2', 'yada']
  excSto = ['labyrinth', 'ssca2', 'yada']
  print  printfmt_int("bench", "type", ['starts', 'reads', 'writes', 'search', 'check_reads', 'avg_set', 'max_set', 'avg_rset', 'max_rset'], width)
          
  for b in bench:
    tl2stat = run_tl2(b, configs[b])
    print printfmt_int(b, 'tl2', tl2stat, width)
    genstmstat = run_sto(b, configs[b], "gen")
    print printfmt_int(b, "genSTM", genstmstat, width)
    if not b in excSto:
      stostat = run_sto(b, configs[b], "STO")
      print printfmt_int(b, "STO", stostat, width)
