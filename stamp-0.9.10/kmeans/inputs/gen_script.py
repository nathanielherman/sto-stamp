from generate import generate

for numRow in [2**i for i in range(10, 17)]:
    file = 'random%d-d16-c20.txt' % numRow
    output = open(file, 'w')
    generate(numRow, 16, 20, output)


