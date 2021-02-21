cd src/
make clean
make

cd ../app/
rm -rf ../platformstats
make clean
make
cp platformstats ../

