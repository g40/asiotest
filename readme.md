
#### Synchronous ASIO and OpenSSL example ####

ASIO is a rather large and sprawling affair, made more complicated by having 11 ways of doing the same thing, in both synchronous and asynchronous fashion.

This pair of examples shows, I hope, the absolute minimum amount of code to get a working SSL connection between a client and server, the latter acting as an echo service.

##### Windows #####

For Windows/Visual Studio, setup is critical and extremely tedious unless vcpkg is used.

For this sort of gig I like everything to be contained in a single binary, hence static linkage for both runtime and support libraries. The VS2022 project files are set up accordingly. See the vcpkg tab in the solution explorer.

Step by step:

1. git clone https://github.com/microsoft/vcpkg.git
2. cd vcpkg
3. bootstrap-vcpkg.bat
4. vcpkg integrate install
5. vcpkg install asio:x64-windows-static
6. vcpkg install openssl:x64-windows-static
7. Open asiotest.sln
8. Rebuild.

##### Linux/MacOS #####

Ensure libraries are installed:

1. sudo apt install libasio-dev libssl-dev libczmq-dev libsodium-dev
2. `make -C ./server`
3. `make -C ./client`

##### Operation #####

1. Run makcert.cmd (makcert.sh on Linux/MacOS) to generate required (self-signed) certificates. This is a once-off affair and may take some time.
2. Run the server: `server 3456`
3. Run the client: `client localhost 3456`
4. Send simple text messages, `quit` to shutdown client.
