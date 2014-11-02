import subprocess
import numpy

t = 5

subprocess.call(['make', '-f', 'Makefile.STO', 'clean'])
subprocess.call(['make', '-f', 'Makefile.seq'])
cmd = ['./vacation', '-c1', '-n2', '-q90', '-u98', '-r1048576', '-t4194304']

for i in range(t):
    cmd[1] = '-c'+ str(n)
    out = subprocess.Popen(cmd, stdout=subprocess.PIPE).communicate()[0]
    out = out.split('\n')[11]
    out = out.split('=')
    time.append(float(out[1]))
print numpy.mean(time)
print numpy.std(time)


subprocess.call(['make', '-f', 'Makefile.STO', 'clean'])
subprocess.call(['make', '-f', 'Makefile.STO'])
cmd = ['./vacation', '-c1', '-n2', '-q90', '-u98', '-r1048576', '-t4194304']

for n in [1, 2, 4, 8, 16, 32]:
    time = []
    for i in range(t):
        cmd[1] = '-c'+ str(n)
        out = subprocess.Popen(cmd, stdout=subprocess.PIPE).communicate()[0]
        out = out.split('\n')[11]
        out = out.split('=')
        time.append(float(out[1]))
    print numpy.mean(time)
    print numpy.std(time)
