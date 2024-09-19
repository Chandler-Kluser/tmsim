ifdef MINGW
CC		= x86_64-w64-mingw32-gcc
CFLAGS += -DMINGW
else
ifndef CLANG
CC      = gcc
else
CC      = clang
endif
endif
SRC 	= src
OBJ 	= obj
BIN		= bin
BUILD 	= tmsim

CFLAGS += -Iinc
ifndef DEBUG
CFLAGS += -O2
endif
ifdef DEBUG
CFLAGS += -g
endif
ifdef OPENMP
CFLAGS += -DOPENMP -fopenmp
endif

SRCS = $(wildcard $(SRC)/*.c)
OBJS = $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))

.phony: all shared clean install

all: $(BIN)/$(BUILD)

$(BIN)/$(BUILD): $(OBJS) | $(BIN)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJ)/%.o: $(SRC)/%.c | $(OBJ)
	$(CC) $(CFLAGS) -c $< -o $@

bin:
	mkdir -p $(BIN)

obj:
	mkdir -p $(OBJ)

clean:
	rm -rf $(BIN) $(OBJ)

install:
	@echo "Install path to /usr/bin"
	cp $(BIN)/$(BUILD) /usr/bin