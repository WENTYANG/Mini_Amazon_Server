CC=g++
CFLAGS=-g
PROTO_CC = ./protobuf/world_amazon.pb.cc ./protobuf/AUprotocolV3.pb.cc
PROTO_H = ./protobuf/world_amazon.pb.h ./protobuf/AUprotocolV3.pb.h
GPB = `pkg-config --cflags --libs protobuf`
VERSION = -std=c++11
THREAD = -pthread
PQXX = -lpqxx -lpq
CPPFILES = main.cpp server.cpp socket.cpp threadpool.cpp sql_functions.cpp WorldHandle.cpp UpsHandle.cpp
HEADERS = server.h exception.h socket.h proto.h threadpool.h sql_functions.h ThreadSafe_queue.h responseHandler.h WorldHandle.h UpsHandle.h


all: server

server:  $(HEADERS) $(PROTO_H) $(CPPFILES)
	$(CC) $(CFLAGS)  -o server $(CPPFILES) $(PROTO_CC) $(GPB) $(PQXX) $(THREAD) $(VERSION)

.PHONY:
	clean
clean:
	rm -rf *.o server  