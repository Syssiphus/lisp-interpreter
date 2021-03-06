# scheme project makefile
# author: bpaschen@googlemail.com
# date: 2015 Nov 11
# log:
#      - 2015 Nov 11: Created makefile

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
# CC  = gcc 
CC  = clang
else
CC = gcc
endif

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

CFLAGS = $(PROFFLAG) -Isrc -Wall -Werror -std=c99
LDFLAGS = $(PROFLINK) -lgc -ldl -lm 

# libPCRE
CFLAGS += `pkg-config --cflags libpcre`
LDFLAGS += `pkg-config --libs libpcre`

# garbage collector
CFLAGS += `pkg-config --cflags bdw-gc`
LDFLAGS += `pkg-config --libs bdw-gc`

ifeq ($(DBG), 1)
CFLAGS += $(PROFFLAG) -g -O0 -D_DEBUG
LDFLAGS +=
else
CFLAGS += $(PROFFLAG) -O2
LDFLAGS +=
endif

all: $(TARGET)

legacy: $(LEGACY_TARGET)

clean:
	rm -f $(TARGET) $(LEGACY_TARGET)
	rm -f $(OBJS) $(LEGACY_OBJS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)

$(LEGACY_TARGET): $(LEGACY_OBJS)
	$(CC) -o $(LEGACY_TARGET) $(LEGACY_OBJS) $(LDFLAGS)


