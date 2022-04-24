CC=g++
CFLAGS=-g
PROTO_CC = ./protobuf/world_amazon.pb.cc ./protobuf/AUprotocolV4.pb.cc
PROTO_H = ./protobuf/world_amazon.pb.h ./protobuf/AUprotocolV4.pb.h
GPB = `pkg-config --cflags --libs protobuf`
PROTO_LIB_DIR = ./protobuf/lib
PROTO_HEADER_DIR = ./protobuf/include
POCO_H = ./Poco/Foundation/include/Poco/Thread.h ./Poco/Foundation/include/Poco/Timer.h
POCO_LIB_DIR = ./Poco/lib
POCO_HEADER_DIR = ./Poco/Foundation/include 
POCO_HEADER_DIR_NET = ./Poco/Net/include
POCO_HEADER_DIR_SSL = ./Poco/NetSSL_OpenSSL/include
POCO_HEADER_DIR_CRYPTO = ./Poco/Crypto/include
POCO_HEADER_DIR_UTIL = ./Poco/Util/include
VERSION = -std=c++11
THREAD = -pthread
PQXX = -lpqxx -lpq
CPPFILES = main.cpp server.cpp socket.cpp threadpool.cpp sql_functions.cpp WorldHandle.cpp UpsHandle.cpp
HEADERS = server.h exception.h socket.h proto.h threadpool.h sql_functions.h ThreadSafe_queue.h WorldHandle.h UpsHandle.h
CPP_MORE = warehouse.cpp working_threads.cpp timer_handle.cpp util.cpp
HEADER_MORE = warehouse.h working_threads.h timer_handle.h util.h

all: server

server:  $(HEADERS) $(PROTO_H) $(POCO_H) $(CPPFILES) $(CPP_MORE) $(HEADER_MORE)
	$(CC) $(CFLAGS)  $(CPPFILES) $(CPP_MORE) $(PROTO_CC) -o server -I$(PROTO_HEADER_DIR) -L$(PROTO_LIB_DIR) -lprotobuf -I$(POCO_HEADER_DIR_UTIL) -I$(POCO_HEADER_DIR_CRYPTO) -I$(POCO_HEADER_DIR) -I$(POCO_HEADER_DIR_NET) -I$(POCO_HEADER_DIR_SSL) -L$(POCO_LIB_DIR) -lPocoNet -lPocoNetSSL -lPocoUtil -lPocoFoundation $(PQXX) $(THREAD) $(VERSION)

.PHONY:
	clean
clean:
	rm -rf *.o server  
