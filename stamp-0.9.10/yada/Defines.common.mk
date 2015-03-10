# ==============================================================================
#
# Defines.common.mk
#
# ==============================================================================


CFLAGS += -DLIST_NO_DUPLICATES
CFLAGS += -DMAP_USE_HASHTABLE
CFLAGS += -DSET_USE_MAP

PROG := yada
SRCS += \
	coordinate.c \
	element.c \
	mesh.c \
	region.c \
	yada.c \
	$(LIB)/avltree.c \
	$(LIB)/heap.c \
	$(LIB)/hashtable.c\
	$(LIB)/list.c \
	$(LIB)/mt19937ar.c \
	$(LIB)/pair.c \
	$(LIB)/queue.c \
	$(LIB)/random.c \
	$(LIB)/rbtree.c \
	$(LIB)/thread.c \
	$(LIB)/vector.c \

#
OBJS := ${SRCS:.c=.o}


# ==============================================================================
#
# End of Defines.common.mk
#
# ==============================================================================
