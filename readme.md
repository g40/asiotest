
#### ASIO and OpenSSL example ####

For Windows/Visual Studio, setup is critical and extremely tedious unless vcpkg is used:

1 git clone https://github.com/Microsoft/vcpkg
2 cd vcpkg
3 bootstrap-vcpkg.bat
4 vcpkg integrate install
5 vcpkg install asio:x64-windows-static
6 vcpkg install openssl:x64-windows-static
7 Open asiotest.sln
8 Rebuild.
