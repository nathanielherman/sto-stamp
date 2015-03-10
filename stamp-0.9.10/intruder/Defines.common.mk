# ==============================================================================
#
# Defines.common.mk
#
# ==============================================================================


PROG := intruder

SRCS += \
	decoder.c \
	detector.c \
	dictionary.c \
	intruder.c \
	packet.c \
	preprocessor.c \
	stream.c \
	$(LIB)/list.c \
	$(LIB)/mt19937ar.c \
	$(LIB)/pair.c \
	$(LIB)/pair2keycompare.c \
	$(LIB)/queue.c \
	$(LIB)/random.c \
	$(LIB)/rbtree.c \
	$(LIB)/thread.c \
	$(LIB)/vector.c \
	$(LIB)/hashtable.c
#
OBJS := ${SRCS:.c=.o}

CFLAGS += -DMAP_USE_HASHTABLE


# ==============================================================================
#
# End of Defines.common.mk
#
# ==============================================================================
