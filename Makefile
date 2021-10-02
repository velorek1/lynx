.POSIX:
CC      = cc
CFLAGS  = -Wall -Wextra -Os
LDFLAGS =
LDLIBS  =

obj = demo.o scbuf.o tm.o rterm.o  

demo: $(obj)
	$(CC) $(LDFLAGS) -o $@ $(obj) $(LDLIBS)

clean:
	rm -f demo $(obj)

demo.o: demo.c scbuf.h rterm.h tm.h
scbuf.o: scbuf.c rterm.h scbuf.h
tm.o: tm.c tm.h
rterm.o: rterm.c rterm.h
