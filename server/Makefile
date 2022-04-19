CC=g++
CFLAGS=-g
PROTO_CC = ./protobuf/world_amazon.pb.cc ./protobuf/AUprotocolV3.pb.cc
PROTO_H = ./protobuf/world_amazon.pb.h ./protobuf/AUprotocolV3.pb.h
GPB = `pkg-config --cflags --libs protobuf`
VERSION = -std=c++11
THREAD = -pthread
PQXX = -lpqxx -lpq
CPPFILES = main.cpp server.cpp socket.cpp threadpool.cpp sql_functions.cpp WorldHandle.cpp UpsHandle.cpp
HEADERS = server.h exception.h socket.h proto.h threadpool.h sql_functions.h ThreadSafe_queue.h WorldHandle.h UpsHandle.h
CPP_MORE = warehouse.cpp working_threads.cpp
HEADER_MORE = warehouse.h working_threads.h

all: server

server:  $(HEADERS) $(PROTO_H) $(CPPFILES) $(CPP_MORE) $(HEADER_MORE)
	$(CC) $(CFLAGS)  -o server $(CPPFILES) $(PROTO_CC) $(GPB) $(PQXX) $(THREAD) $(VERSION) $(CPP_MORE) $(HEADER_MORE)

.PHONY:
	clean
clean:
	rm -rf *.o server  
