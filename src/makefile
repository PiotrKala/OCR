CC=gcc
PTHREAD=-lpthread
CCFLAGS=-w

all: main

main:
	$(CC) $(CCFLAGS) -o main main.c LibBMP.c  skeleton.c extractor.c $(PTHREAD)

clean:
	rm main

test:
	./main ../examples/sample.bmp
	@echo 
	cat output.txt
	@echo
	cat final.txt
