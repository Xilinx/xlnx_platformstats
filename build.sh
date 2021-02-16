cd src/
make clean
make

cd ../app/
rm -rf ../main
make clean
make
cp main ../

