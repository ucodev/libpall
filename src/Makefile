CC=`cat ../.compiler`
CCFLAGS=-fPIC -fstrict-aliasing -O2 -Wall -Werror -pedantic -std=c11
ECFLAGS=`cat ../.ecflags`
LDFLAGS=-shared -s
ELFLAGS=`cat ../.elflags`
TARGET=libpall.so

all:
	${CC} ${CCFLAGS} ${ECFLAGS} -c bst.c
	${CC} ${CCFLAGS} ${ECFLAGS} -c cll.c
	${CC} ${CCFLAGS} ${ECFLAGS} -c fifo.c
	${CC} ${CCFLAGS} ${ECFLAGS} -c hmbt_bst.c
	${CC} ${CCFLAGS} ${ECFLAGS} -c hmbt_cll.c
	${CC} ${CCFLAGS} ${ECFLAGS} -c lifo.c
	${CC} ${CCFLAGS} ${ECFLAGS} -c mm.c
	${CC} ${LDFLAGS} -o ${TARGET} bst.o cll.o fifo.o hmbt_bst.o hmbt_cll.o lifo.o mm.o ${ELFLAGS}

clean:
	rm -f *.o
	rm -f ${TARGET}
