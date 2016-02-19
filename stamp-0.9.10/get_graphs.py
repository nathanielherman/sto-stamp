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
            [ {'seq' :    22.5, 'sto' :    6.092, 'stm' :    6.194, 'gen' :    7.019}, #bayes --
             {'seq' :    6.394, 'sto' :    1.631, 'stm' :    2.288, 'gen' :    12.21}, #genome --
             {'seq' : 78.77, 'sto' : 28.62 , 'stm' : 162.7, 'gen' : 270.3}, #intruder --
             {'seq' : 29.44, 'sto' : 13.82, 'sto1' : 14.5, 'stm' : 16.24, 'gen' : 28.93}, #vacation --
             {'seq' : 3.164, 'sto' : 4.357, 'sto1' : 4.156, 'stm' : 13.22, 'gen' : 29.89}, #vacation2 --
             {'seq' :    19.92, 'sto' :    5.037, 'stm'  : 7.539, 'gen' :    11.58}, #kmeans --
             {'seq' :    54.43, 'sto' :    12.15, 'stm' :    17.52, 'gen' :    16.22}, #labyrinth --
             {'seq' :    34.94, 'stm' :    21.27, 'gen' :    20.69}], #ssca2 --
              #t = 16
            [ {'seq' :    22.5, 'sto' :    3.824, 'stm' :    5.591, 'gen' :    9.639}, #bayes --
             {'seq' :    6.394, 'sto' :    0.6364, 'stm' :    1.255, 'gen' :    11.35}, #genome --
             {'seq' : 78.77, 'sto' : 11.1, 'stm' : 145.1, 'gen' : 279.0}, #intruder --
             {'seq' : 29.44, 'sto' : 5.795, 'sto1' : 5.943, 'stm' : 7.774, 'gen' : 24.65}, #vacation --
             {'seq' : 3.164, 'sto' : 2.406, 'sto1' : 1.805, 'stm' : 15.98, 'gen' : 33.8}, #vacation2 --
             {'seq' :    19.92, 'sto' :    1.722, 'stm' : 2.309, 'gen' :    5.144}, #kmeans --
             {'seq' :    54.43, 'sto' :     5.431, 'stm' :    9.074, 'gen' :    7.111}, #labyrinth --
             {'seq' :    34.94, 'stm' :    10.12, 'gen' :    10.09}] #ssca2 --
            ]


min_ = [  # t = 4
          [ {'seq' :    22.5, 'sto' :    4.89, 'stm' :    3.896, 'gen' :    4.103}, #bayes --
            {'seq' :    6.394, 'sto' :    1.627, 'stm' :    2.267, 'gen' :    11.9}, #genome --
             {'seq' : 78.77, 'sto' : 28.57, 'stm' : 157.5, 'gen' : 269.6}, #intruder --
             {'seq' : 29.44, 'sto' : 13.7, 'sto1' : 14.18, 'stm' : 16.19, 'gen' : 28.82}, #vacation --
             {'seq' : 3.164, 'sto' : 4.319, 'sto1' : 4.151, 'stm' : 13.15, 'gen' : 29.8}, #vacation2 --
             {'seq' :    19.92, 'sto' :    4.599, 'stm' : 7.172, 'gen' :    11.37}, #kmeans --
             {'seq' :    54.43, 'sto' :    12.08, 'stm' :    16.88, 'gen' :    15.92}, #labyrinth --
             {'seq' :    34.94, 'stm' :    21.25, 'gen' :    20.38}], #ssca2 --
            #t = 16
            [ {'seq' :    22.5, 'sto' :    3.191, 'stm' :    5.519, 'gen' :    7.22}, #bayes --
             {'seq' :    6.394, 'sto' :    0.5884, 'stm' :    1.229, 'gen' :    10.76}, #genome --
             {'seq' : 78.77, 'sto' : 11.06, 'stm' : 139.7, 'gen' : 276.9}, #intruder --
             {'seq' : 29.44, 'sto' : 5.078, 'sto1' : 4.867, 'stm' : 6.818, 'gen' : 24.59}, #vacation --
             {'seq' : 3.164, 'sto' : 2.254, 'sto1' : 1.694, 'stm' : 15.71, 'gen' : 33.54}, #vacation2 --
             {'seq' :    19.91, 'sto' :     1.456, 'stm' : 2.126, 'gen' :    4.151}, #kmeans --
             {'seq' :    54.43, 'sto' :    5.356, 'stm' :    8.524, 'gen' :    6.889},#labyrinth --
             {'seq' :    34.94, 'stm' :     10.11, 'gen' :    10.02}] #ssca2 --
            ]
max_ = [  # t = 4
            [ {'seq' :    22.5, 'sto' :    8.06, 'stm' :    12.4, 'gen' :    12.49}, #bayes --
             {'seq' :    6.394, 'sto' :    1.726, 'stm' :    2.296, 'gen' :    12.37}, #genome --
             {'seq' : 78.77, 'sto' : 28.68, 'stm' : 172.5, 'gen' : 272.0}, #intruder --
             {'seq' : 29.44, 'sto' : 13.87, 'sto1' : 14.56, 'stm' : 16.43, 'gen' : 29.18}, #vacation --
             {'seq' : 3.164, 'sto' : 4.53, 'sto1' : 4.169, 'stm' : 13.92, 'gen' : 29.98}, #vacation2 --
             {'seq' :    19.92, 'sto' :    5.87, 'stm' : 8.181, 'gen' :    11.82}, #kmeans --
             {'seq' :    54.43, 'sto' :     12.28, 'stm' :    21.21, 'gen' :    16.57}, #labyrinth --
             {'seq' :    34.94, 'stm' :    21.61, 'gen' :    20.83}], #ssca2 --
            #t = 16
            [ {'seq' :    22.5, 'sto' :    6.237, 'stm' :    24.92, 'gen' :    17.96}, #bayes --
             {'seq' :    6.394, 'sto' :     0.6701, 'stm' :    1.314, 'gen' :    13.3}, #genome --
             {'seq' : 78.77, 'sto' : 11.11, 'stm' : 147.8, 'gen' : 279.5}, #intruder --
             {'seq' : 29.44, 'sto' : 6.277, 'sto1' : 7.117, 'stm' : 9.275, 'gen' : 25.17}, #vacation --
             {'seq' : 3.164, 'sto' : 2.445, 'sto1' : 2.419, 'stm' : 16.28, 'gen' : 34.09}, #vacation2 --
             {'seq' :    19.92, 'sto' :    1.916, 'stm' : 2.509, 'gen' :    5.812}, #kmeans --
             {'seq' :    54.43, 'sto' :    5.543, 'stm' :    9.318, 'gen' :    7.387}, #labyrinth --
             {'seq' :    34.94, 'stm' :     10.16, 'gen' :    10.1}] #ssca2 --
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
