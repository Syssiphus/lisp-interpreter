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
		  src/object.c src/memory.c src/builtins.c src/environment.c
CXX_SOURCES = 
OBJS = $(SOURCES:.c=.o) $(CXX_SOURCES:.cpp=.o)

LEGACY_SOURCES = legacy_src/main.c
LEGACY_OBJS = $(LEGACY_SOURCES:.c=.o)

ifeq ($(PROF), 1)
PROFFLAG = -pg
PROFLINK = -pg
else
PROFFLAG =
PROFLINK =
endif

ifeq ($(DBG), 1)
CFLAGS = $(PROFFLAG) -g -O0 -Isrc -Wall -ansi -D_DEBUG
CXXFLAGS = $(PROFFLAG) -g -O0 -Isrc -Wall -D_DEBUG
LDFLAGS = $(PROFLINK) -lgc -ldl -lpcre
else
CFLAGS = $(PROFFLAG) -O2 -Isrc -Wall -ansi
CXXFLAGS = $(PROFFLAG) -O2 -Isrc -Wall
LDFLAGS = $(PROFLINK) -lgc -ldl -lpcre
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


