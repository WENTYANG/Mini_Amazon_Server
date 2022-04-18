CC=g++
CFLAGS=-g
PROTO_CC = ./protobuf/world_amazon.pb.cc
PROTO_H = ./protobuf/world_amazon.pb.h
GPB = `pkg-config --cflags --libs protobuf`
VERSION = -std=c++11
THREAD = -pthread
PQXX = -lpqxx -lpq
CPPFILES = main.cpp server.cpp socket.cpp threadpool.cpp
HEADERS = server.h exception.h socket.h proto.h threadpool.h


all: server

server:  $(HEADERS) $(PROTO_H) $(CPPFILES)
	$(CC) $(CFLAGS)  -o server $(CPPFILES) $(PROTO_CC) $(GPB) $(PQXX) $(THREAD) $(VERSION)

.PHONY:
	clean
clean:
	rm -rf *.o server  