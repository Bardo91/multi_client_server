//---------------------------------------------------------------------------------------------------------------------
//  HECATONQUIROS
//---------------------------------------------------------------------------------------------------------------------
//  Copyright 2018 - Pablo Ramon Soria (a.k.a. Bardo91) - University of Seville
//---------------------------------------------------------------------------------------------------------------------
//  Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
//  and associated documentation files (the "Software"), to deal in the Software without restriction, 
//  including without limitation the rights to use, copy, modify, merge, publish, distribute, 
//  sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is 
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all copies or substantial 
//  portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
//  BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
//  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES 
//  OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
//  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//---------------------------------------------------------------------------------------------------------------------

#include<iostream>
#include <boost/asio.hpp>

namespace mcs {
	//-----------------------------------------------------------------------------------------------------------------
	template<>
	MultiClientServer<eSocketType::TCP>::MultiClientServer(eSocketType _type, int _port) {
		mSocketServer = new SocketServer<eSocketType::TCP>(_port);
		
	}

	//-----------------------------------------------------------------------------------------------------------------
	template<>
	template<>
	void MultiClientServer<eSocketType::TCP>::writeOnClients<std::string>(std::string &_data) {
		assert(mSocketType == eSocketType::TCP);
		mSocketServer->writeOnClients(_data);
	}

	//-----------------------------------------------------------------------------------------------------------------
	template<>
	template<typename T_>
	void MultiClientServer<eSocketType::TCP>::writeOnClients(T_ &_data) {
		assert(mSocketType == eSocketType::UDP);

	}


	//-----------------------------------------------------------------------------------------------------------------
	template<>
	template<>
	MultiClientServer<eSocketType::TCP>::SocketServer<eSocketType::TCP>::SocketServer(int _port) {
		mPort = _port;
		mRun = true;
		std::cout << "Creating TCP server...";
		mListenThread = std::thread([&]() {
			try {
				boost::asio::io_service io_service;
				boost::asio::ip::tcp::acceptor acceptor(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), mPort));

				std::cout << "awaiting for connetions." << std::endl;
				while (mRun) {
					boost::asio::ip::tcp::socket *socket = new boost::asio::ip::tcp::socket(io_service);
					acceptor.accept(*socket);
					std::cout << "Received new connection: " << socket->native_handle() << std::endl;

					mSafeGuard.lock();
					mTcpConnections.push_back(socket);
					mSafeGuard.unlock();
				}
			}
			catch (std::exception &e) {
				std::cerr << e.what() << std::endl;
			}
		});
	}

	//-----------------------------------------------------------------------------------------------------------------
	template<>
	template<>
	void MultiClientServer<eSocketType::TCP>::SocketServer<eSocketType::TCP>::stop() {
		mRun = false;
	}

	//-----------------------------------------------------------------------------------------------------------------
	template<>
	template<>
	void MultiClientServer<eSocketType::TCP>::SocketServer<eSocketType::TCP>::writeOnClients(std::string &_buffer) {
		try {
			mSafeGuard.lock();
			for (auto &con : mTcpConnections) {
				boost::system::error_code error;
				if (con->is_open())
					boost::asio::write(*con, boost::asio::buffer(_buffer), error);
			}
			mSafeGuard.unlock();
		}
		catch (std::exception &e) {
			std::cerr << e.what() << std::endl;
			mSafeGuard.unlock();
		}
	}
}