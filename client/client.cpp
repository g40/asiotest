/*

    A super simple application to demonstrate authentication and Curve security with plain ZeroMQ

    Setup is critical.

    git clone https://github.com/Microsoft/vcpkg
    cd vcpkg
    ls
    bootstrap-vcpkg.bat
    vcpkg install zeromq:x64-windows-static
    vcpkg install libsodium:x64-windows-static
    vcpkg install zeromq[curve]:x64-windows-static
    vcpkg install zeromq[sodium]:x64-windows-static
    vcpkg remove zeromq[sodium]:x64-windows-static
    vcpkg install zeromq[sodium]:x64-windows-static
    vcpkg install zeromq[sodium]:x64-windows-static

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

//-----------------------------------------------------------------------------
enum { max_length = 1024 };

//-----------------------------------------------------------------------------
bool verify_certificate(bool preverified,asio::ssl::verify_context& ctx)
{
    // The verify callback can be used to check whether the certificate that is
    // being presented is valid for the peer. For example, RFC 2818 describes
    // the steps involved in doing this for HTTPS. Consult the OpenSSL
    // documentation for more details. Note that the callback is called once
    // for each certificate in the certificate chain, starting from the root
    // certificate authority.

    // In this example we will simply print the certificate's subject name.
    char subject_name[256];
    X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
    X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
    std::cout << "Verifying " << subject_name << "\n";
    return preverified;
}

//-----------------------------------------------------------------------------
// https://stackoverflow.com/questions/40036854/creating-a-https-request-using-boost-asio-and-openssl
// https://stackoverflow.com/questions/28264313/ssl-certificates-and-boost-asio
// 
// 
//-----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	int ret = -1;
	try
	{
        if (argc != 3)
        {
            throw std::runtime_error("Usage: client <host> <port>");
        }

        // common
        asio::io_context io_context;
        using ssl_socket = asio::ssl::stream<asio::ip::tcp::socket>;

        std::array<char, 128> buf;
        asio::error_code error;

        if (1)
        {
            // Create a context that uses the default paths for
            // finding CA certificates.
            asio::ssl::context ctx(asio::ssl::context::sslv23);
            ctx.set_default_verify_paths();
            ctx.load_verify_file("certs/rootca.pem");
            //ctx.set_verify_mode(asio::ssl::verify_peer);

            ssl_socket socket(io_context, ctx);
            asio::ip::tcp::resolver resolver(io_context);

            //
            asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(argv[1], argv[2]);
            asio::connect(socket.lowest_layer(), endpoints);

            socket.lowest_layer().set_option(asio::ip::tcp::no_delay(true));
            // Perform SSL handshake and verify the remote host's
            // certificate.

            // socket.set_verify_callback(asio::ssl::host_name_verification(argv[1]));
            socket.set_verify_callback(&verify_certificate);
            socket.handshake(ssl_socket::client);

            for (;;)
            {
                std::cout << "Enter message: ";
                std::string message;
                std::getline(std::cin,message);

                size_t len = asio::write(socket, asio::buffer(message));
                std::cout << " wrote " << len << " (" << message.size() << ")" << std::endl;
                if (message == "quit")
                {
                    socket.lowest_layer().close();
                    break;
                }

                //
                len = socket.read_some(asio::buffer(buf), error);
                if (len == 0 || error == asio::error::eof)
                {
                    std::cout << "connection closed by host" << std::endl;
                    break; // Connection closed cleanly by peer.
                }
                else if (error)
                {
                    throw asio::system_error(error); // Some other error.
                }
                //
                std::cout << " read " << len << std::endl;
                std::string s(buf.data(), buf.data() + len);
                std::cout << s << std::endl;
            }
        }
        else
        {
            // all synchronous, blocking
            asio::ip::tcp::resolver resolver(io_context);
            asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(argv[1], argv[2]);

            asio::ip::tcp::socket socket(io_context);
            asio::connect(socket, endpoints);
            for (;;)
            {
                std::cout << "Enter message: ";
                std::string message;
                std::getline(std::cin, message);

                size_t len = asio::write(socket, asio::buffer(message));
                std::cout << " wrote " << len << " (" << message.size() << ")" << std::endl;
                if (message == "quit")
                {
                    //socket.shutdown_both();
                    socket.close();
                    break;
                }

                len = socket.read_some(asio::buffer(buf), error);
                if (len == 0)
                {
                    std::cout << "connection closed by host" << std::endl;
                    break; // Connection closed cleanly by peer.
                }
                else if (error)
                {
                    throw asio::system_error(error); // Some other error.
                }
                std::cout << " read " << len << std::endl;
                std::string s(buf.data(), buf.data()+len);
                std::cout << s << std::endl;
            }
        }

        ret = 0;
	}
	catch (const std::exception& ex)
	{
		std::cout << "Error: " << ex.what() << std::endl;
	}
	return ret;
}
