CC=`cat ../.compiler`
INCLUDEDIRS=-I../include
CCFLAGS=-fPIC -fstrict-aliasing -O2 -Wall -Werror -pedantic
ECFLAGS=`cat ../.ecflags`
LDFLAGS=-shared -s
ELFLAGS=`cat ../.elflags`
TARGET=libpall.so

all:
	${CC} ${INCLUDEDIRS} ${CCFLAGS} ${ECFLAGS} -c bst.c
	${CC} ${INCLUDEDIRS} ${CCFLAGS} ${ECFLAGS} -c cll.c
	${CC} ${INCLUDEDIRS} ${CCFLAGS} ${ECFLAGS} -c fifo.c
	${CC} ${INCLUDEDIRS} ${CCFLAGS} ${ECFLAGS} -c hmbt_bst.c
	${CC} ${INCLUDEDIRS} ${CCFLAGS} ${ECFLAGS} -c hmbt_cll.c
	${CC} ${INCLUDEDIRS} ${CCFLAGS} ${ECFLAGS} -c lifo.c
	${CC} ${INCLUDEDIRS} ${CCFLAGS} ${ECFLAGS} -c mm.c
	${CC} ${LDFLAGS} -o ${TARGET} bst.o cll.o fifo.o hmbt_bst.o hmbt_cll.o lifo.o mm.o ${ELFLAGS}

clean:
	rm -f *.o
	rm -f ${TARGET}
