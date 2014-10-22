from generate import generate

for numRow in [2**i for i in range(10, 17)]:
    file = 'random%d-d12-c20.txt' % numRow
    output = open(file, 'w')
    generate(numRow, 12, 12, output)


