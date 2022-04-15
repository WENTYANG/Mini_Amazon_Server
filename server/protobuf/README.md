# protobuf usage guide
1. unzip protobuf-cpp-3.20.0.zip
2. cd protobuf-cpp-3.20.0.zip
3. following github install instruction to compile and install protobuf <https://github.com/protocolbuffers/protobuf/blob/main/src/README.md>
4. use protobuf compiler to create class file for each *.proto file.  
   using `protoc -I=$SRC_DIR --cpp_out=$DST_DIR $SRC_DIR/addressbook.proto`  
   if you are in the same director as *.proto files, `-I=$SRC_DIR`can be ignored.
5. compile your program and link to protobuf library.  
    `g++ -o writeExample writeExample.cpp addressbook.pb.cc 'pkg-config --cflags --libs protobu'`  
    Notice: ' means backticks(~)
