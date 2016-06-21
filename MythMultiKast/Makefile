CXX=g++
CC=gcc
CPPFLAGS=-I. -I/usr/include/SDL2 -I/usr/local/include/groupsock -I/usr/local/include/UsageEnvironment -I/usr/local/include/BasicUsageEnvironment -I/usr/local/include/liveMedia -fpermissive -Wdeprecated-declarations -w -O3
CPPSOURCES=$(wildcard *.cpp)
CPPOBJECTS=$(CPPSOURCES:.cpp=.o)
CSOURCES=$(wildcard *.c)
COBJECTS=$(CSOURCES:.c=.o)
TARGET=mythmultikast

.PHONY: all clean

all: .dcpp .dc $(CPPSOURCES) $(CSOURCES) $(TARGET)

.dcpp: $(CPPSOURCES)
	$(CXX) $(CPPFLAGS) -MM $(CPPSOURCES) >.dcpp
.dc: $(CSOURCES)
	$(CC) $(CFLAGS) -MM $(CSOURCES) >.dc
-include .dcpp
-include .dc
$(TARGET): $(CPPOBJECTS) $(COBJECTS)
	$(CXX) $(CPPOBJECTS) $(COBJECTS) -o $@ -L/usr/lib -lSDL2 -lSDL2_net -L/usr/local/lib/ -lliveMedia -lBasicUsageEnvironment -lgroupsock -lUsageEnvironment -lavcodec -lavfilter -lavutil -lswresample -lswscale -lavdevice -lavformat -lpostproc 

clean:
	rm $(CPPOBJECTS) $(COBJECTS) .dcpp .dc
install:
	cp $(TARGET) /usr/local/bin
