CXX=g++
CC=gcc
CPPFLAGS=-I. -I/usr/include/SDL2 -O3 -std=c++0x
CFLAGS=-I. -I/usr/include/SDL2  -O3
CPPSOURCES=$(wildcard *.cpp)
#CPPSOURCES=main.cpp mythRedis.cpp mythRedisKey.cpp mythBaseServer.cpp mythLua.cpp mythReader.cpp mythVirtualServer.cpp mythWriter.cpp PEOPLE.cpp
CPPOBJECTS=$(CPPSOURCES:.cpp=.o)
#CSOURCES=$(wildcard *.c)
COBJECTS=$(CSOURCES:.c=.o)
TARGET=grandserver

.PHONY: all clean

all: .dcpp .dc $(CPPSOURCES) $(CSOURCES) $(TARGET)

.dcpp: $(CPPSOURCES)
	$(CXX) $(CPPFLAGS) -MM $(CPPSOURCES) >.dcpp
.dc: $(CSOURCES)
	$(CC) $(CFLAGS) -MM $(CSOURCES) >.dc
-include .dcpp
-include .dc
$(TARGET): $(CPPOBJECTS) $(COBJECTS)
	$(CXX) $(CPPOBJECTS) $(COBJECTS) -o $@ -L/usr/lib -lSDL2 -lSDL2_net

clean:
	rm $(CPPOBJECTS) $(COBJECTS) .dcpp .dc