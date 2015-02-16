CC=`cat ../.compiler`
CCFLAGS=-I../include -std=c99 -fstrict-aliasing -O2 -Wall -Werror -pedantic
LDFLAGS=../src/libpall.so -s
ELFLAGS=`cat ../.elflags`
ARCHFLAGS=`cat ../.archflags`

all:
	${CC} ${CCFLAGS} ${ARCHFLAGS} -c eg_bst_simple.c
	${CC} -o eg_bst_simple eg_bst_simple.o ${LDFLAGS} ${ELFLAGS}
	${CC} ${CCFLAGS} ${ARCHFLAGS} -c eg_cll_simple.c
	${CC} -o eg_cll_simple eg_cll_simple.o ${LDFLAGS} ${ELFLAGS}
	${CC} ${CCFLAGS} ${ARCHFLAGS} -c eg_cll_config.c
	${CC} -o eg_cll_config eg_cll_config.o ${LDFLAGS} ${ELFLAGS}
	${CC} ${CCFLAGS} ${ARCHFLAGS} -c eg_hmbt-bst_simple.c
	${CC} -o eg_hmbt-bst_simple eg_hmbt-bst_simple.o ${LDFLAGS} ${ELFLAGS}
	${CC} ${CCFLAGS} ${ARCHFLAGS} -c eg_hmbt-cll_simple.c
	${CC} -o eg_hmbt-cll_simple eg_hmbt-cll_simple.o ${LDFLAGS} ${ELFLAGS}
	${CC} ${CCFLAGS} ${ARCHFLAGS} -c eg_hmbt-cll_config.c
	${CC} -o eg_hmbt-cll_config eg_hmbt-cll_config.o ${LDFLAGS} ${ELFLAGS}
	${CC} ${CCFLAGS} ${ARCHFLAGS} -c eg_fifo_simple.c
	${CC} -o eg_fifo_simple eg_fifo_simple.o ${LDFLAGS} ${ELFLAGS}
	${CC} ${CCFLAGS} ${ARCHFLAGS} -c eg_lifo_simple.c
	${CC} -o eg_lifo_simple eg_lifo_simple.o ${LDFLAGS} ${ELFLAGS}

clean:
	rm -f *.o
	rm -f eg_bst_simple
	rm -f eg_cll_simple
	rm -f eg_cll_config
	rm -f eg_hmbt-bst_simple
	rm -f eg_hmbt-cll_simple
	rm -f eg_hmbt-cll_config
	rm -f eg_fifo_simple
	rm -f eg_lifo_simple

