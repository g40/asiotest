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


using asio::ip::tcp;

class session : public std::enable_shared_from_this<session>
{
public:
    session(asio::ssl::stream<tcp::socket> socket)
        : socket_(std::move(socket))
    {
    }

    void start()
    {
        do_handshake();
    }

private:
    void do_handshake()
    {
        auto self(shared_from_this());
        socket_.async_handshake(asio::ssl::stream_base::server,
            [this, self](const std::error_code& error)
            {
                if (!error)
                {
                    do_read();
                }
            });
    }

    void do_read()
    {
        auto self(shared_from_this());
        socket_.async_read_some(asio::buffer(data_),
            [this, self](const std::error_code& ec, std::size_t length)
            {
                std::string msg(data_,data_+length);
                std::cout << "Incoming: " << msg << std::endl;
                if (!ec)
                {
                    do_write(length);
                }
            });
    }

    void do_write(std::size_t length)
    {
        auto self(shared_from_this());
        asio::async_write(socket_, asio::buffer(data_, length),
            [this, self](const std::error_code& ec,
                std::size_t /*length*/)
            {
                if (!ec)
                {
                    do_read();
                }
            });
    }

    asio::ssl::stream<tcp::socket> socket_;
    char data_[1024];
};

//---------------------------------------------------------------
class server
{
public:
    server(asio::io_context& io_context, unsigned short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
        context_(asio::ssl::context::sslv23)
    {
        context_.set_options(
            asio::ssl::context::default_workarounds
            | asio::ssl::context::no_sslv2
            | asio::ssl::context::single_dh_use);
        context_.set_password_callback(std::bind(&server::get_password, this));
        context_.use_certificate_chain_file("server.pem");
        context_.use_private_key_file("server.pem", asio::ssl::context::pem);
        context_.use_tmp_dh_file("dh4096.pem");

        do_accept();
    }

private:
    std::string get_password() const
    {
        return "test";
    }

    void do_accept()
    {
        acceptor_.async_accept(
            [this](const std::error_code& error, tcp::socket socket)
            {
                std::cout << "Incoming: " << socket.remote_endpoint() << std::endl;
                if (!error)
                {
                    std::make_shared<session>(
                        asio::ssl::stream<tcp::socket>(
                            std::move(socket), context_))->start();
                }
                do_accept();
            });
    }

    tcp::acceptor acceptor_;
    asio::ssl::context context_;
};

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

        asio::io_context io_context;
        server s(io_context, std::atoi(argv[1]));
        io_context.run();
        //io_context.stop();
        ret = 0;
    }
    catch (const std::exception& ex)
    {
        std::cout << "Error: " << ex.what() << std::endl;
    }
    return ret;
}
