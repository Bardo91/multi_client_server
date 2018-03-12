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

#include <functional>
#include <thread>
#include <mutex>
#include <vector>
#include <boost/asio.hpp>


namespace mcs {
	class MultiClientServer {
	public:
		enum eSocketType {UDP, TCP};

		/// Initialize server that allows multiple connection from different clients
		/// \param _port: port to listen the connections
		MultiClientServer(eSocketType _type, int _port);

		/// Add a custom handler
		void addHandler(std::function<void(int)> &_handler);

		/// Add a custom callback
		void addCallback(std::function<void(int)> &_callback);

		/// Write message
		template<typename T_>
		void writeOnClients(T_ &_data);

	private:
		class TcpSocketServer {
		public:
			TcpSocketServer(int  _port);

			void writeOnClients(std::string &_buffer);

			void stop();
		private:
			std::thread mListenThread;
			bool mRun = false;

			//std::vector<boost::asio::ip::tcp::socket> mConnections;
		
			std::mutex mSafeGuard;
		};

	private:
		eSocketType mSocketType = eSocketType::TCP;
		TcpSocketServer *mTcpServer;
	};

}