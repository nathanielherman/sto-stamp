# ==============================================================================
#
# Defines.common.mk
#
# ==============================================================================


CFLAGS += -DLIST_NO_DUPLICATES=1 -DMAP_USE_RBTREE -DBM_VACATION

PROG := vacation

SRCS += \
	client.c \
	customer.c \
	manager.c \
	reservation.c \
	vacation.c \
	$(LIB)/list.c \
	$(LIB)/pair.c \
	$(LIB)/mt19937ar.c \
	$(LIB)/random.c \
	$(LIB)/rbtree.c \
	$(LIB)/thread.c \
	$(LIB)/hashtable.c \


# ==============================================================================
#
# End of Defines.common.mk
#
# ==============================================================================
