cc = g++
library_dir = 
include_dir = 
libs = -lpthread
objs = main.o functions.o md5.o bytearray.o rtsp_util.o rtsp_client.o rtsp_server.o rtp_protocol.o base64.o \
	   sps_decode.o
exe = example
c_flags = -g -std=c++11

$(exe):$(objs)
	$(cc) $(objs) $(c_flags) -o $(exe) $(libs) $(library_dir)
main.o:
	$(cc) -c $(c_flags) main.cpp
md5.o:
	$(cc) -c $(c_flags) src/md5.cpp
base64.o:
	$(cc) -c $(c_flags) src/base64.cpp
bytearray.o:
	$(cc) -c $(c_flags) src/bytearray.cpp
functions.o:
	$(cc) -c $(c_flags) src/functions.cpp
rtsp_util.o:
	$(cc) -c $(c_flags) src/rtsp_util.cpp
rtsp_client.o:
	$(cc) -c $(c_flags) src/rtsp_client.cpp
rtsp_server.o:
	$(cc) -c $(c_flags) src/rtsp_server.cpp
rtp_protocol.o:
	$(cc) -c $(c_flags) src/rtp_protocol.cpp
sps_decode.o:
	$(cc) -c $(c_flags) src/sps_decode.cpp

.PHONY:clean
clean:
	rm -f *.o
clean-all:
	rm -f *.o example
