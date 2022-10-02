# In order to avail of Parvicrursor infrastructure and examples, you require to firstly install Crypto++ library by instructions given below

# To install Crypto++ in Linux, run the following commands within the shell:

# First download it
wget https://www.cryptopp.com/cryptopp860.zip

# Then, make it from source code
unzip cryptopp860.zip -d cryptopp-linux
cd cryptopp-linux
make libcryptopp.a libcryptopp.so cryptest.exe
make install
ldconfig


# To clean the installation, use the following command:
make clean