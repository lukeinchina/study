CC=g++
CFLAGS=-g -Wall -Wextra

#conv Neural Networks
OBJS= util.o conv_layer.o 
all: conv_layer_test 

%.o: %.cc
	$(CC) -c -o $@ $< $(CFLAGS)

conv_layer_test: $(OBJS) conv_layer_test.o 
	$(CC) -o $@ $^ $(CFLAGS)

#minst_test: $(OBJS) minst_test.o 
	#$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -f *.o  conv_layer_test 
