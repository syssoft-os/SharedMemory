CC = gcc
CFLAGS = -Wall -g
LIBS = -lm
TARGETS = $(patsubst %.c,%,$(wildcard *.c))

all: $(TARGETS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

clean:
	rm -f $(TARGETS)