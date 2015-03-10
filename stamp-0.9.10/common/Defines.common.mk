# ==============================================================================
#
# Defines.common.mk
#
# ==============================================================================


CC       := gcc 
#CC       := g++ -std=c++11
CFLAGS   += -g -Wall -pthread
CFLAGS   += -O3
CFLAGS   += -I$(LIB)
CPP      := g++ --std=c++11
CPPFLAGS += $(CFLAGS)
LD       := g++
LIBS     += -lpthread

# Remove these files when doing clean
OUTPUT +=

LIB := ../lib

STM := ../../tl2

LOSTM := ../../OpenTM/lostm


# ==============================================================================
#
# End of Defines.common.mk
#
# ==============================================================================
