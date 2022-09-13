./bootstrap
./configure --enable-fstack --disable-shared
make
sudo make install

cd examples
make
./make_client.sh
