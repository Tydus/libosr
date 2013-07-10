SRCS := parser.c main.c
OBJS := $(SRCS:.c=.o)
OUT  := osrc

override CFLAGS += -std=c99 -Wall
CFLAGS += -O2

all: $(OUT)

tags: *.c *.h
	ctags -R

osrc: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJS): %.o:%.c
	$(CC) -c $(CFLAGS) -o $@ $<

clean: 
	$(RM) $(OBJS)

distclean: clean
	$(RM) $(OUT) tags

.PHONY: all clean distclean
