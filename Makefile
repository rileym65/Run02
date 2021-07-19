PROJECT = run02
CC = gcc
DEFS =
INCS =
INCDIR =
LIBS =
LIBDIR =
OBJS = \
	cpu.o \
	loader.o \
	main.o \
	trace.o

$(PROJECT): $(OBJS)
	$(CC) $(DEFS) $(LIBDIR) $(OBJS) $(LIBS) -o $(PROJECT)

.c.o:
	$(CC) $(DEFS) $(INCDIR) $(INCS) -c $<

clean:
	-rm *.o
	-rm $(PROJECT)

install:
	cp run02 /usr/local/bin
	chmod a+x /usr/local/bin/run02

cpu.o:           header.h cpu.c
loader.o:        header.h loader.c
main.o:          header.h main.c
trace.o:         header.h trace.c
