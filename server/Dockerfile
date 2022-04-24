FROM ubuntu:20.04
RUN mkdir /code
ADD . /code/
WORKDIR /code

ARG DEBIAN_FRONTEND=noninteractive
ENV TZ=Etc/UTC
RUN ls 
RUN apt-get update 
RUN apt-get -y install build-essential libpqxx-dev postgresql openssl libssl-dev
ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/code/Poco/lib
RUN export LD_LIBRARY_PATH
RUN make

# # install protobuf
# RUN cd protobuf-3.20.0 && ./configure && make && make check && make install && ldconfig

RUN chmod u+r+x run.sh