CC=`cat ../.compiler`
INCLUDEDIRS=-I../include
CCFLAGS=
ECFLAGS=`cat ../.ecflags`
LDFLAGS=-shared -s
ELFLAGS=`cat ../.elflags`
ARCHFLAGS=`cat ../.archflags`
TARGET=libpall.`cat ../.extlib`

all:
	${CC} ${INCLUDEDIRS} ${CCFLAGS} ${ECFLAGS} ${ARCHFLAGS} -c bst.c
	${CC} ${INCLUDEDIRS} ${CCFLAGS} ${ECFLAGS} ${ARCHFLAGS} -c cll.c
	${CC} ${INCLUDEDIRS} ${CCFLAGS} ${ECFLAGS} ${ARCHFLAGS} -c fifo.c
	${CC} ${INCLUDEDIRS} ${CCFLAGS} ${ECFLAGS} ${ARCHFLAGS} -c hmbt_bst.c
	${CC} ${INCLUDEDIRS} ${CCFLAGS} ${ECFLAGS} ${ARCHFLAGS} -c hmbt_cll.c
	${CC} ${INCLUDEDIRS} ${CCFLAGS} ${ECFLAGS} ${ARCHFLAGS} -c lifo.c
	${CC} ${INCLUDEDIRS} ${CCFLAGS} ${ECFLAGS} ${ARCHFLAGS} -c mm.c
	${CC} ${LDFLAGS} -o ${TARGET} bst.o cll.o fifo.o hmbt_bst.o hmbt_cll.o lifo.o mm.o ${ELFLAGS}

clean:
	rm -f *.o
	rm -f ${TARGET}
