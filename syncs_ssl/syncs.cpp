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
// for pass-phrase protected cert files
std::string get_password()
{
	return "test";
}

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

		// SSL context
		// Create a context that uses fixed paths for CA certificates.
		asio::ssl::context ssl_context(asio::ssl::context::sslv23);
		ssl_context.set_options(asio::ssl::context::default_workarounds
			| asio::ssl::context::no_sslv2
			| asio::ssl::context::single_dh_use);
		// get it automatically
		ssl_context.set_password_callback(std::bind(&get_password));
		ssl_context.use_certificate_chain_file("server.pem");
		ssl_context.use_private_key_file("server.pem", asio::ssl::context::pem);
		ssl_context.use_tmp_dh_file("dh4096.pem");

		// IO context
		asio::io_context io_context;

		// acceptor for incoming connections
		asio::ip::tcp::endpoint ep(asio::ip::tcp::v4(), port);
		asio::ip::tcp::acceptor acceptor(io_context, ep);

		using ssl_socket = asio::ssl::stream<asio::ip::tcp::socket>;
		ssl_socket socket(io_context, ssl_context);

		//
		std::cout << "Waiting: " << ep.address() << ":" << ep.port() << std::endl;
		// waiting for connection
		acceptor.accept(socket.lowest_layer());
		//
		std::cout << "Incoming: " << socket.lowest_layer().remote_endpoint() << std::endl;

		// socket.lowest_layer().set_option(asio::ip::tcp::no_delay(true));
		// Perform SSL handshake and verify the remote host's
		// certificate.
		socket.set_verify_mode(asio::ssl::verify_peer);
		socket.set_verify_callback(asio::ssl::host_name_verification(argv[1]));
		socket.handshake(ssl_socket::server);

		// should now be able to read/write?
		ret = 0;
	}
	catch (const std::exception& ex)
	{
		std::cout << "Error: " << ex.what() << std::endl;
	}
	return ret;
}
