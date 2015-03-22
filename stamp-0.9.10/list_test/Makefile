CPP = g++
LD = $(CPP)
CFLAGS = -std=c++11 -g -Wall
LDFLAGS = -lpthread
OPT = -O3
INCPATH = ../lib
SYMBOLS = -DSTO -DLIST_NO_DUPLICATES
CFLAGS += $(OPT) -I$(INCPATH) $(SYMBOLS)

MAINSRC = list_test_race.cpp
DEPSRC = ../lib/thread.c
OBJS = $(MAINSRC:.cpp=.o) $(DEPSRC:.c=.o)
TARGET = $(MAINSRC:.cpp=)

%.o: %.cpp
	$(CPP) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CPP) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(LD) $(CFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)

all: $(TARGET)

clean:
	rm -vf $(TARGET) $(OBJS)
