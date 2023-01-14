PROG = project05
OBJS = project05.o parsetwo.o

CFLAGS: = -g

%.o : %.c
	gcc $(CFLAGS) -c -o $@ $<

$(PROG) : project05.h $(OBJS)
	gcc $(CFLAGS) -o $@ $(OBJS)

all : $(PROG)

clean:
       rm -rf $(PROG) $(OBJS)
