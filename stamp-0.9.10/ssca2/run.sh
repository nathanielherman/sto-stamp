RESDIR="./tmp"
echo "Creating tmp directory " $RESDIR
mkdir $RESDIR

CONFIG="-s21 -i1.0 -u1.0 -l3 -p3"

make -f Makefile.seq clean > /dev/null 2>&1
make -f Makefile.seq > /dev/null 2>&1

echo "Seq run "$CONFIG
./ssca2 $CONFIG > $RESDIR/seq.txt

python getTime.py $RESDIR/seq.txt

make -f Makefile.STO clean > /dev/null 2>&1
make -f Makefile.STO > /dev/null 2>&1

for t in 1 2 4 8 16
do
echo "STO run "$CONFIG" -t"$t
./ssca2 $CONFIG -t$t > $RESDIR/sto$t.txt
python getTime.py $RESDIR/sto$t.txt
done


make -f Makefile.stm clean > /dev/null 2>&1
make -f Makefile.stm > /dev/null 2>&1

for t in 1 2 4 8 16
do
echo "TL2 run "$CONFIG" -t"$t
./ssca2 $CONFIG -t$t > $RESDIR/stm$t.txt
python getTime.py $RESDIR/stm$t.txt
done

