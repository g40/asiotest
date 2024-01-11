/*

    A super simple application to demonstrate authentication using ASIO

*/


#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include <openssl/opensslconf.h>
#include <asio.hpp>
#include <asio/ssl.hpp>

#ifdef WIN32
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "crypt32.lib")
#endif

#ifdef _DEBUG
#pragma comment(lib, "libssl.lib")
#else
#pragma comment(lib, "libssl.lib")
#endif


//---------------------------------------------------------------
int main(int argc, char* argv[])
{
    int ret = -1;
    try
    {
        if (argc != 2)
        {
            throw std::runtime_error("Usage: server <port>");
        }

        int port = std::atoi(argv[1]);
        asio::io_context io_context;

        asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), port);
        asio::ip::tcp::acceptor acceptor(io_context, endpoint);
        asio::ip::tcp::socket socket(io_context);
        // waiting for connection
        acceptor.accept(socket);
        //
        std::cout << "Incoming: " << socket.remote_endpoint() << std::endl;

        std::array<char, 128> buf;
        asio::error_code error;

        for (;;)
        {
            size_t len = socket.read_some(asio::buffer(buf), error);
            std::string message(buf.data(), buf.data() + len);
            std::cout << "Read: " << len << " => " << message << std::endl;
            if (message.find("quit") == 0)
            {
                socket.close();
                break;
            }
            // read operation
            //asio::streambuf buf;
            // this does block as expected
            //asio::read_until(socket, buf, "t");
            //std::string message = asio::buffer_cast<const char*>(buf.data());
            std::cout << message << std::endl;
            //write operation
            asio::write(socket, asio::buffer("Hello world\n"));
        }
        // shutdown
        acceptor.close();
    }
    catch (const std::exception& ex)
    {
        std::cout << "Error: " << ex.what() << std::endl;
    }
    return ret;
}
