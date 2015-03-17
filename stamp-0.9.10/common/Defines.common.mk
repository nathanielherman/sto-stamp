# ==============================================================================
#
# Defines.common.mk
#
# ==============================================================================


CC       := gcc
OPT      := -O3
CFLAGS   += -g -Wall -pthread
CFLAGS   += $(OPT)
CFLAGS   += -I$(LIB)
CXX      := g++ -std=c++11
CXXFLAGS += $(CFLAGS)
LD       = $(CXX) $(CXXFLAGS)
LIBS     += -lpthread

DEPSDIR  := .deps
OBJDIR   := obj
DEPCFLAGS = -MD -MF $(DEPSDIR)/$*.d -MP

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
