CC=gcc
CFLAGS=-std=c99 -Wall -Wextra
LOAD=Load_Balancer
SERVER=Server
CDLL=CircularDoublyLinkedList
HASHRING=Hash_Ring
LIST=LinkedList
HASH=Hashtable

.PHONY: build clean

build: tema2
tema2: main.o $(LOAD).o $(SERVER).o $(LIST).o $(HASHRING).o $(CDLL).o $(HASH).o
	$(CC) $^ -o $@

main.o: main.c
	$(CC) $(CFLAGS) $^ -c

$(SERVER).o: $(SERVER).c $(SERVER).h
	$(CC) $(CFLAGS) $^ -c

$(LOAD).o: $(LOAD).c $(LOAD).h
	$(CC) $(CFLAGS) $^ -c

$(CDLL).o: $(CDLL).c $(CDLL).h
	$(CC) $(CFLAGS) $^ -c

$(HASHRING).o: $(HASHRING).c $(HASHRING).h
	$(CC) $(CFLAGS) $^ -c

$(LIST).o: $(LIST).c $(LIST).h
	$(CC) $(CFLAGS) $^ -c
$(HASH).o: $(HASH).c $(HASH).h
	$(CC) $(CFLAGS) $^ -c
clean:
	rm -f *.o tema2 *.h.gch
