CC=gcc
OUTPUT=sim

sim: main.c
	$(CC) -o $(OUTPUT) main.c

clean: $(OUTPUT)
	rm -rf $(OUTPUT)
