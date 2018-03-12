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

#include <MultiClientServer.h>

#include <iostream>
#include <cassert>
#include <string>

using boost::asio::ip::tcp;

namespace mcs {

	//-----------------------------------------------------------------------------------------------------------------
	MultiClientServer::MultiClientServer(MultiClientServer::eSocketType _type, int _port) {
		mSocketType = _type;
		switch (_type) {
		case mcs::MultiClientServer::UDP:
			assert(false);
			break;
		case mcs::MultiClientServer::TCP:
			mTcpServer = new TcpSocketServer(_port);
			break;
		default:
			assert(false);
			break;
		}
	}

	//-----------------------------------------------------------------------------------------------------------------
	void MultiClientServer::addHandler(std::function<void(int)> &_handler) {

	}

	//-----------------------------------------------------------------------------------------------------------------
	void MultiClientServer::addCallback(std::function<void(int)> &_callback) {

	}

	//-----------------------------------------------------------------------------------------------------------------
	template<>
	void MultiClientServer::writeOnClients<std::string>(std::string &_data) {
		assert(mSocketType == eSocketType::TCP);
		mTcpServer->writeOnClients(_data);
	}

	//-----------------------------------------------------------------------------------------------------------------
	template<typename T_>
	void MultiClientServer::writeOnClients(T_ &_data) {
		assert(mSocketType == eSocketType::UDP);
		
	}

	//-----------------------------------------------------------------------------------------------------------------
	// PRIVATE CLASSES
	//-----------------------------------------------------------------------------------------------------------------
	MultiClientServer::TcpSocketServer::TcpSocketServer(int _port) {
		mRun = true;
		std::cout << "Creating TCP server...";
		mListenThread = std::thread([&]() {
			try {
				boost::asio::io_service io_service;
				boost::asio::ip::tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), _port));

				std::cout << "awaiting for connetions." << std::endl;
				while (mRun) {
					tcp::socket socket(io_service);
					acceptor.accept(socket);
					std::cout << "Received new connection: " << socket.native_handle() << std::endl;

					mSafeGuard.lock();
					//mConnections.push_back(socket);
					mSafeGuard.unlock();
				}
			}
			catch (std::exception &e) {
				std::cerr << e.what() << std::endl;
			}
		});
	}

	//-----------------------------------------------------------------------------------------------------------------
	void MultiClientServer::TcpSocketServer::stop() {
		mRun = false;
	}

	//-----------------------------------------------------------------------------------------------------------------
	void MultiClientServer::TcpSocketServer::writeOnClients(std::string &_buffer) {
		try {
			mSafeGuard.lock();
			//for (auto &con : mConnections) {
			//	boost::system::error_code error;
			//	boost::asio::write(con, boost::asio::buffer(_buffer), error);
			//}
			mSafeGuard.unlock();
		}
		catch (std::exception &e) {
			std::cerr << e.what() << std::endl;
			mSafeGuard.unlock();
		}
	}
}