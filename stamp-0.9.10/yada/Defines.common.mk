# ==============================================================================
#
# Defines.common.mk
#
# ==============================================================================


CFLAGS += -DLIST_NO_DUPLICATES
# Note that changing this will break things. AVLTREE expects a pair comparison
# function so if you change to a different structure you must change all
# MAP_ALLOC's to pass the listCompare function rather than the mapCompare function
CFLAGS += -DMAP_USE_AVLTREE
# This is OK to change
CFLAGS += -DSET_USE_HASHTABLE

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
