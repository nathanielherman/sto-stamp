#!/usr/bin/env python

import subprocess
import re
import os


if __name__ == "__main__":
  bench = ['bayes', 'kmeans', 'intruder', 'genome', 'labyrinth','ssca2', 'vacation']
  for b in bench :
    print b
    os.chdir('./' + b)
    subprocess.check_output(['python', 'script.py'], stderr=subprocess.STDOUT)
    os.chdir('..')
