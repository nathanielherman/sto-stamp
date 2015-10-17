#!/usr/bin/env python

import subprocess
import re
import os
import numpy as np
import matplotlib.pyplot as plt

tableau20 = [(31, 119, 180), (174, 199, 232), (255, 127, 14), (255, 187, 120),  
             (44, 160, 44), (152, 223, 138), (214, 39, 40), (255, 152, 150),  
             (148, 103, 189), (197, 176, 213), (140, 86, 75), (196, 156, 148),  
             (227, 119, 194), (247, 182, 210), (127, 127, 127), (199, 199, 199),  
             (188, 189, 34), (219, 219, 141), (23, 190, 207), (158, 218, 229)]  
  
# Scale the RGB values to the [0, 1] range, which is the format matplotlib accepts.  
for i in range(len(tableau20)):  
  r, g, b = tableau20[i]  
  tableau20[i] = (r / 255., g / 255., b / 255.)

COLORS = tableau20

try:
  if sys.argv[1] == 'bw':
    COLORS = [(0,0,0), (.5,.5,.5), (.7,.7,.7), (.9,.9,.9)]
except:
  pass

perm = [2, 1, 5, 6, 3, 4, 0, 7]

def permute(l) :
  new_l = []
  for i in perm:
    new_l.append(l[i])
  return new_l


results_ = [  # t = 4
            [ {'seq' :    22.51, 'sto' :    8.334, 'stm' :    8.424, 'gen' :    6.063}, #bayes
             {'seq' :    6.391, 'sto' :    1.652, 'stm' :     2.31, 'gen' :    13.14}, #genome
             {'seq' : 78.84, 'sto' : 28.59 , 'stm' : 161.1, 'gen' : 270.25}, #intruder
             {'seq' : 29.4, 'sto' : 13.59, 'sto1' : 13.68, 'stm' : 16.26, 'gen' : 28.46}, #vacation
             {'seq' : 3.17, 'sto' : 4.15, 'sto1' : 3.65, 'stm' : 14.23, 'gen' : 29.2}, #vacation2
             {'seq' :    30.72, 'sto' :    8.741, 'stm'  : 15.08, 'gen' :    21.51}, #kmeans
             {'seq' :    54.44, 'sto' :    11.96, 'stm' :    17.75, 'gen' :    16.17}, #labyrinth
             {'seq' :    34.86, 'stm' :    21.14, 'gen' :    19.22}], #ssca2
              #t = 16
            [ {'seq' :    22.51, 'sto' :    5.394, 'stm' :    7.442, 'gen' :    16.35}, #bayes
             {'seq' :    6.391, 'sto' :   0.6042, 'stm' :    1.234, 'gen' :    12.59}, #genome
             {'seq' : 78.84, 'sto' : 11.26, 'stm' : 153.0, 'gen' : 276.2}, #intruder
             {'seq' : 29.4, 'sto' : 5.79, 'sto1' : 5.77, 'stm' : 7.58, 'gen' : 23.98}, #vacation
             {'seq' : 3.17, 'sto' : 2.43, 'sto1' : 1.54, 'stm' : 16.85, 'gen' : 32.98}, #vacation2
             {'seq' :    30.72, 'sto' :    2.852, 'stm' : 4.60, 'gen' :    8.947}, #kmeans
             {'seq' :    54.44, 'sto' :     5.33, 'stm' :    8.857, 'gen' :    6.964}, #labyrinth
             {'seq' :    34.86, 'stm' :    10.13, 'gen' :    10.05}] #ssca2
            ]


min_ = [  # t = 4
          [ {'seq' :    22.51, 'sto' :    6.779, 'stm' :    5.441, 'gen' :    4.347}, #bayes
            {'seq' :    6.391, 'sto' :    1.626, 'stm' :     2.28, 'gen' :    11.69}, #genome
             {'seq' : 78.84, 'sto' : 28.53, 'stm' : 147.53, 'gen' : 270.18}, #intruder
             {'seq' : 29.4, 'sto' : 13.49, 'sto1' : 13.56, 'stm' : 16.20, 'gen' : 28.37}, #vacation
             {'seq' : 3.17, 'sto' : 4.13, 'sto1' : 3.45, 'stm' : 14.03, 'gen' : 29.11}, #vacation2
             {'seq' :    30.72, 'sto' :    8.613, 'stm' : 14.53, 'gen' :    21.29}, #kmeans
             {'seq' :    54.44, 'sto' :    11.83, 'stm' :    17.32, 'gen' :    15.99}, #labyrinth
             {'seq' :    34.86, 'stm' :    21.11, 'gen' :    19.09}], #ssca2
            #t = 16
            [ {'seq' :    22.51, 'sto' :    3.049, 'stm' :    4.667, 'gen' :    4.163}, #bayes
             {'seq' :    6.391, 'sto' :   0.5849, 'stm' :    1.217, 'gen' :    12.25}, #genome
             {'seq' : 78.84, 'sto' : 11.20, 'stm' : 146.58, 'gen' : 275.86}, #intruder
             {'seq' : 29.4, 'sto' : 5.13, 'sto1' : 4.72, 'stm' : 6.41, 'gen' : 23.83}, #vacation
             {'seq' : 3.17, 'sto' : 2.34, 'sto1' : 1.48, 'stm' : 16.53, 'gen' : 32.74}, #vacation2
             {'seq' :    30.72, 'sto' :     2.74, 'stm' : 4.59, 'gen' :    8.627}, #kmeans
             {'seq' :    54.44, 'sto' :    5.277, 'stm' :    8.312, 'gen' :    6.821},#labyrinth
             {'seq' :    34.86, 'stm' :     10.1, 'gen' :    10.02}] #ssca2
            ]
max_ = [  # t = 4
            [ {'seq' :    22.51, 'sto' :    10.19, 'stm' :    12.37, 'gen' :    12.66}, #bayes
             {'seq' :    6.391, 'sto' :    1.679, 'stm' :    2.363, 'gen' :    21.18}, #genome
             {'seq' : 78.84, 'sto' : 28.69, 'stm' : 189.59, 'gen' : 271.83}, #intruder
             {'seq' : 29.4, 'sto' : 13.85, 'sto1' : 13.73, 'stm' : 16.42, 'gen' : 28.56}, #vacation
             {'seq' : 3.17, 'sto' : 4.16, 'sto1' : 3.75, 'stm' : 15.37, 'gen' : 29.27}, #vacation2
             {'seq' :    30.72, 'sto' :    8.939, 'stm' : 15.10, 'gen' :    22.13}, #kmeans
             {'seq' :    54.44, 'sto' :     12.1, 'stm' :    20.52, 'gen' :    16.25}, #labyrinth
             {'seq' :    34.86, 'stm' :    21.26, 'gen' :    19.71}], #ssca2
            #t = 16
            [ {'seq' :    22.51, 'sto' :    12.09, 'stm' :    16.79, 'gen' :    18.37}, #bayes
             {'seq' :    6.391, 'sto' :   0.7125, 'stm' :     1.27, 'gen' :    15.92}, #genome
             {'seq' : 78.84, 'sto' : 11.37, 'stm' : 169.54, 'gen' : 278.4}, #intruder
             {'seq' : 29.4, 'sto' : 6.07, 'sto1' : 6.22, 'stm' : 9.98, 'gen' : 24.27}, #vacation
             {'seq' : 3.17, 'sto' : 2.46, 'sto1' : 1.66, 'stm' : 17.39, 'gen' : 33.14}, #vacation2
             {'seq' :    30.72, 'sto' :    2.854, 'stm' : 4.78, 'gen' :    10.78}, #kmeans
             {'seq' :    54.44, 'sto' :    5.449, 'stm' :    8.876, 'gen' :    7.058}, #labyrinth
             {'seq' :    34.86, 'stm' :     10.2, 'gen' :    10.26}] #ssca2
            ]




def plot_graph(ind, t):
  results = results_[ind]
  min = min_[ind]
  max = max_[ind]
  N = len(results)
  width = 0.2
  ind = np.arange(N) + width

  fig = plt.figure(figsize = (8, 4))
  ax = fig.add_subplot(111)

  stmvals = []
  stovals = []
  genvals = []
  sto1vals = []
  
  stmmin = []
  stomin = []
  genmin = []
  sto1min = []
  
  stmmax = []
  stomax = []
  genmax = []
  sto1max = []
  
  for i in range(N) :
    stmvals.append(results[i]['seq'] / results[i]['stm'])
    stmmax.append((min[i]['seq']/min[i]['stm']) - stmvals[i])
    stmmin.append(stmvals[i] - max[i]['seq']/max[i]['stm'])
    genvals.append(results[i]['seq'] / results[i]['gen'])
    genmax.append((min[i]['seq']/min[i]['gen']) - genvals[i])
    genmin.append(genvals[i] - max[i]['seq']/max[i]['gen'])
    if 'sto' in results[i] :
      stovals.append(results[i]['seq'] / results[i]['sto'])
      stomax.append((min[i]['seq']/min[i]['sto']) - stovals[i])
      stomin.append(stovals[i] - max[i]['seq']/max[i]['sto'])
    else :
      stovals.append(0)
      stomin.append(0)
      stomax.append(0)
    if 'sto1' in results[i]:
      sto1vals.append(results[i]['seq'] / results[i]['sto1'])
      sto1max.append((min[i]['seq']/min[i]['sto1']) - sto1vals[i])
      sto1min.append(sto1vals[i] - max[i]['seq']/max[i]['sto1'])
    else :
      sto1vals.append(0)
      sto1min.append(0)
      sto1max.append(0)

  rects1 = ax.bar(ind, permute(stmvals), width, color = COLORS[0], yerr = [permute(stmmin), permute(stmmax)], ecolor = 'k')
  rects2 = ax.bar(ind+width, permute(genvals), width, color = COLORS[1], yerr = [permute(genmin), permute(genmax)], ecolor = 'k')
  rects3 = ax.bar(ind+width*2, permute(stovals), width, color = COLORS[2], yerr = [permute(stomin), permute(stomax)], ecolor = 'k')
  rects4 = ax.bar(ind+width*3, permute(sto1vals), width, color = COLORS[3], yerr = [permute(sto1min), permute(sto1max)], ecolor = 'k')

  ax.set_ylabel('Speedup on ' + str(t) + ' cores')
  ax.set_xticks(ind+width)


  ax.set_xticklabels(('intruder', 'genome', 'kmeans', '  labyrinth', '    vacationL', '      vacationH', 'bayes', 'ssca2'), verticalalignment = 'top')

  ax.legend((rects1[0], rects2[0], rects3[0], rects4[0]), ('TL2', 'TUntyped', 'STO', 'STO+'),  ncol=4, loc='upper right', prop={'size': 10})
  plt.tight_layout()
  plt.savefig('stamp' + str(t) + '.pdf')

if __name__ == "__main__":
  plot_graph(0, 4)
  plot_graph(1, 16)
