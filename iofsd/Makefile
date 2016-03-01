include .knightos/variables.make

AS=scas
ASFLAGS=-I"$(INCLUDE)"

OBJECTS:=$(patsubst src/%.asm,$(OUT)%.o,$(wildcard src/*.asm))

ALL_TARGETS:=$(BIN)iofsd

$(OUT)%.o: src/%.asm
	mkdir -p $(OUT)
	$(AS) $(ASFLAGS) -c $< -o $@

$(OUT)iofsd.o: $(OBJECTS)
	mkdir -p $(OUT)
	$(AS) $(ASFLAGS) -c crt0.asm $(OBJECTS) -o $(OUT)iofsd.o

$(BIN)iofsd: $(OUT)iofsd.o
	mkdir -p $(BIN)
	$(AS) $(ASFLAGS) $(OUT)iofsd.o -o $(BIN)iofsd

include .knightos/sdk.make
