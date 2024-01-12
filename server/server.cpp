/*

	A super simple application to demonstrate authentication using ASIO + OpenSSL

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

		// IO context
		asio::io_context io_context;

		std::array<char, 128> buf;
		asio::error_code error;


		if (1)
		{
			// SSL
			// Create a context that uses fixed paths for CA certificates.
			asio::ssl::context ssl_context(asio::ssl::context::sslv23);
			ssl_context.set_options(asio::ssl::context::default_workarounds
				| asio::ssl::context::no_sslv2
				| asio::ssl::context::single_dh_use);
			// must be a .PEM
			ssl_context.use_certificate_chain_file("certs/user.pem");
			// .PEM or ASN.1
			ssl_context.use_private_key_file("certs/user.key", asio::ssl::context::pem);
			// must be a .PEM
			ssl_context.use_tmp_dh_file("certs/dh4096.pem");

			// acceptor for incoming connections
			asio::ip::tcp::endpoint ep(asio::ip::tcp::v4(), port);
			asio::ip::tcp::acceptor acceptor(io_context, ep);

			using ssl_socket = asio::ssl::stream<asio::ip::tcp::socket>;

			for (;;)
			{
				// 
				ssl_socket socket(io_context, ssl_context);
				//
				std::cout << "Waiting: " << ep.address() << ":" << ep.port() << std::endl;
				// wait for connection
				acceptor.accept(socket.lowest_layer());
				//
				std::cout << "Incoming: " << socket.lowest_layer().remote_endpoint() << std::endl;
				// should now be able to read/write?
				// Perform SSL handshake and verify the remote host's
				// certificate.
				socket.set_verify_mode(asio::ssl::verify_peer);
				socket.set_verify_callback(asio::ssl::host_name_verification(argv[1]));
				socket.handshake(ssl_socket::server);

				for (;;)
				{
					// read operation
					size_t len = socket.read_some(asio::buffer(buf), error);
					if (len == 0 || error)
					{
						break;
					}

					std::string message(buf.data(), buf.data() + len);
					std::cout << "Read: " << len << " => " << message << std::endl;
					if (message.find("quit") == 0)
					{
						socket.lowest_layer().close();
						break;
					}
					// write operation
					asio::write(socket, asio::buffer(message));
				}
			}
		}
		else
		{
			//
			asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), port);
			asio::ip::tcp::acceptor acceptor(io_context, endpoint);

			for (;;)
			{
				asio::ip::tcp::socket socket(io_context);
				// waiting for connection
				acceptor.accept(socket);
				//
				std::cout << "Incoming: " << socket.remote_endpoint() << std::endl;

				size_t len = socket.read_some(asio::buffer(buf), error);
				std::string message(buf.data(), buf.data() + len);
				std::cout << "Read: " << len << " => " << message << std::endl;
				if (message.find("quit") == 0)
				{
					socket.close();
					break;
				}
				std::cout << message << std::endl;
				//write operation
				asio::write(socket, asio::buffer("Hello world\n"));
			}
			// shutdown
			acceptor.close();
		}
		ret = 0;
	}
	catch (const std::exception& ex)
	{
		std::cout << "Error: " << ex.what() << std::endl;
	}
	//
	return ret;
}
