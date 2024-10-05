SRC = src
CC = cc
CFLAGS = -std=c99 -Wall -Wextra

.PHONY: clean xpc

xpc: $(SRC)/main.c
	@echo Compiling...
	$(CC) $(CFLAGS) -o $@ $<
	strip $@
	@echo Done!

install: xpc
	@echo Installing...
	@mkdir -p /usr/local/bin
	@cp $< /usr/local/bin/
	@echo Done!

clean:
	@echo Cleaning up...
	rm -rf xpc
	@echo Done!
