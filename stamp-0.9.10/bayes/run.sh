RESDIR="./tmp"
echo "Creating tmp directory " $RESDIR
mkdir $RESDIR

make -f Makefile.seq clean > /dev/null 2>&1
make -f Makefile.seq > /dev/null 2>&1

echo "Seq run -v32 -r4096 -n10 -p40 -i2 -e8 -s1"
./bayes -v32 -r4096 -n10 -p40 -i2 -e8 -s1 > $RESDIR/seq.txt

python getTime.py $RESDIR/seq.txt

make -f Makefile.STO clean > /dev/null 2>&1
make -f Makefile.STO > /dev/null 2>&1

for t in 1 2 4 8 16 32
do
echo "STO run -v32 -r4096 -n10 -p40 -i2 -e8 -s1 -t"$t
./bayes -v32 -r4096 -n10 -p40 -i2 -e8 -s1 -t $t > $RESDIR/sto$t.txt
python getTime.py $RESDIR/sto$t.txt
done


make -f Makefile.stm clean > /dev/null 2>&1
make -f Makefile.stm > /dev/null 2>&1

for t in 1 2 4 8 16 32
do
echo "TL2 run -v32 -r4096 -n10 -p40 -i2 -e8 -s1 -t"$t
./bayes -v32 -r4096 -n10 -p40 -i2 -e8 -s1 -t $t > $RESDIR/stm$t.txt
python getTime.py $RESDIR/stm$t.txt
done

