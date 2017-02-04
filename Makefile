CC=g++
CFLAGS=-W -Wall -fexceptions -finline-functions -lquickfix
DEPS = recorder.h
OBJ = recorder.cpp

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

recorder: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

all: recorder

clean:
	rm -f recorder *.o
