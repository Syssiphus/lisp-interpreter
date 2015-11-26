# Lispy makefile
# author: bpaschen@googlemail.com
# date: 2015 Nov 11
# log:
#      - 2015 Nov 11: Created makefile

CC = gcc
CXX = g++

TARGET = scheme
LEGACY_TARGET = legacy_scheme

SOURCES = src/scheme.c src/init.c src/eval.c src/read.c src/write.c \
		  src/object.c src/memory.c
CXX_SOURCES = 
OBJS = $(SOURCES:.c=.o) $(CXX_SOURCES:.cpp=.o)

LEGACY_SOURCES = src/main.c
LEGACY_OBJS = $(LEGACY_SOURCES:.c=.o)

ifeq ($(DBG), 1)
CFLAGS = -g -O0 -Isrc -Wall -ansi
CXXFLAGS = -g -O0 -Isrc -Wall
LDFLAGS =
else
CFLAGS = -O2 -Isrc -Wall -ansi
CXXFLAGS = -O2 -Isrc -Wall
LDFLAGS =
endif

all: $(TARGET)

legacy: $(LEGACY_TARGET)

clean:
	rm -f $(TARGET) $(LEGACY_TARGET)
	rm -f $(OBJS) $(LEGACY_OBJS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

%.o: %.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)

$(LEGACY_TARGET): $(LEGACY_OBJS)
	$(CC) -o $(LEGACY_TARGET) $(LEGACY_OBJS) $(LDFLAGS)


