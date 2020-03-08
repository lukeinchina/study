CC=g++
CFLAGS=-g -Wall -Wextra

#ficial Neural Networks
OBJS= node.o network.o
all: check_network minst_test

%.o: %.cc
	$(CC) -c -o $@ $< $(CFLAGS)


check_network: $(OBJS) check_network.o 
	$(CC) -o $@ $^ $(CFLAGS)

minst_test: $(OBJS) minst_test.o 
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -f *.o  check_network minst_test *.h.gch
