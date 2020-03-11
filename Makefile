cc = g++
library_dir = 
include_dir = 
libs = -lpthread
objs = main.o kgr_utils.o kgr_md5.o kgr_bytes.o rtsp_util.o rtsp_client.o rtsp_server.o rtp_protocol.o kgr_base64.o \
	   sps_decode.o rtsp_task.o kgr_logger.o
exe = example
c_flags = -g -std=c++11

$(exe):$(objs)
	$(cc) $(objs) $(c_flags) -o $(exe) $(libs) $(library_dir)
main.o:
	$(cc) -c $(c_flags) main.cpp
kgr_utils.o:
	$(cc) -c $(c_flags) src/common/kgr_utils.cpp
kgr_bytes.o:
	$(cc) -c $(c_flags) src/common/kgr_bytes.cpp
kgr_logger.o:
	$(cc) -c $(c_flags) src/common/kgr_logger.cpp

kgr_md5.o:
	$(cc) -c $(c_flags) src/encrypt/kgr_md5.cpp
kgr_base64.o:
	$(cc) -c $(c_flags) src/encrypt/kgr_base64.cpp

rtsp_task.o:
	$(cc) -c $(c_flags) src/rtsp_task.cpp
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
