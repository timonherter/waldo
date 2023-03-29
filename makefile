CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -ljpeg

all: hide_waldo find_waldo

hide_waldo: hide_waldo.c
	$(CC) $(CFLAGS) hide_waldo.c -o hide_waldo $(LDFLAGS)

find_waldo: find_waldo.c
	$(CC) $(CFLAGS) find_waldo.c -o find_waldo $(LDFLAGS)

clean:
	@rm hide_waldo find_waldo

test: hide_waldo find_waldo
	@echo "\nTesting..."
	@./hide_waldo message.txt input.jpg output.jpg
	@./find_waldo output.jpg decoded.txt
	@diff message.txt decoded.txt
	@echo "\nMessage was encoded and decoded successfully!\n"
	@$(MAKE) clean
