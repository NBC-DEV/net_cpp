#pragma once

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/bind.hpp>

#include "AsioNetDef.h"
#include "BlockBuffer.h"

namespace AsioNet
{
	
	// doc:https://www.boost.org/doc/libs/1_84_0/doc/html/boost_asio/reference/ip__tcp/socket.html

	// 如果以接口的形式返回给上面，我应该如何做到在使用接口的时候其指针的有效性得到保证，shared_ptr?
	struct AN_INTERFACE IANConn : public boost::noncopyable
	{
		virtual bool Write(const char *acData, int iSize) = 0;
		virtual bool Read() = 0;
	};

	struct IConnectHandler
	{
		virtual void operator()(const error_code &error) = 0;
		virtual ~IConnectHandler(){};
	};
	struct IAcceptHandler
	{
		virtual void operator()(const error_code &error) = 0;
		virtual ~IAcceptHandler(){};
	};
	struct IWriteHandler
	{
		virtual void operator()(const error_code &error, std::size_t bytes_transferred) = 0;
		virtual ~IWriteHandler(){};
	};
	struct IReadHandler
	{
		virtual void operator()(const error_code &error, std::size_t bytes_transferred) = 0;
		virtual ~IReadHandler(){};
	};

	struct AN_INTERFACE IANCompleteHandler
	{
		virtual ~IANCompleteHandler() {}
		virtual void connect_handler(const error_code &error) = 0;
		virtual void accept_handler(const error_code &error) = 0;
		virtual void read_handler(const error_code &error, std::size_t bytes_transferred) = 0;
		virtual void write_handler(const error_code &error, std::size_t bytes_transferred) = 0;
	};

	// 只能由client/server创建
	class TcpConn : public std::enable_shared_from_this<TcpConn>
	{
		friend class TcpClient;
		friend class TcpServer;

	public:
		void Close();
		bool Write(const char *data, size_t trans);
		void StartRead(); // start read loop

		TcpConn(io_context &ctx);		 // for client
		TcpConn(ip::tcp::socket &&sock); // for server
		
	protected:
		void read_head_handler(const error_code &, size_t);
		void read_body_handler(const error_code &, size_t);
		void write_handler(const error_code &, size_t);
		void (*err_handler)(const error_code &);
		void (*net_proc)(const char *data, size_t trans);

	private:
		ip::tcp::socket sock_;
		std::mutex sendLock;
		BlockSendBuffer<DEFAULT_SEND_BUFFER_SIZE, DEFAULT_SEND_BUFFER_POOL_EXTEND_SIZE> sendBuffer;
		char readBuffer[1 << (sizeof(AN_Msg::len) * 8)];
	};

	// NewTCPConn()
	// NewUDPConn()
	// NewKCPConn()
	// class TcpConnFactory {
	//
	//};

}
