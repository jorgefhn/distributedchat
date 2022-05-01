BIN_FILES  = servidor

CC = gcc


CFLAGS =	-Wall  -g

LDFLAGS = -L$(INSTALL_PATH)/lib/
LDLIBS = -lpthread


all: $(BIN_FILES)
.PHONY : all


servidor: servidor.o lines.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) $(CFLAGS) -c $<

clean:
	rm -f $(BIN_FILES) *.o

.SUFFIXES:
.PHONY : clean
