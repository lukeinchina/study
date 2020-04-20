CC=g++
CFLAGS=-g -Wall -Wextra

#conv Neural Networks
OBJS= util.o conv_layer.o  max_pooling.o cnn.o node.o network.o
all: conv_layer_test  max_pooling_test cnn_minst

%.o: %.cc
	$(CC) -c -o $@ $< $(CFLAGS)

conv_layer_test: $(OBJS) conv_layer_test.o
	$(CC) -o $@ $^ $(CFLAGS)

max_pooling_test: $(OBJS) max_pooling_test.o
	$(CC) -o $@ $^ $(CFLAGS)

cnn_minst: $(OBJS) cnn_minst.o
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -f *.o  conv_layer_test max_pooling_test
