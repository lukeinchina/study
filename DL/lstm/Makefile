CC=g++
CFLAGS=-g -Wall -Wextra

#conv Neural Networks
OBJS= util.o lstm_layer.o 
all: check_lstm

%.o: %.cc
	$(CC) -c -o $@ $< $(CFLAGS)

check_lstm: $(OBJS) check_lstm.o
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -f *.o  check_lstm
