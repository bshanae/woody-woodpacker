LOG=$(info [woody-woodpacker] $1)
SILENT=@

СС=gcc
CC_FLAGS=-no-pie -I c/include -c 

ASM=nasm
ASM_FLAGS=-f elf64

LD=gcc
LD_FLAGS=-no-pie

EXE=woody_woodpacker

SRC_C_DIR=c/src
SRC_C=$(wildcard c/src/*)
OBJ_C_DIR=obj/c
OBJ_C=$(SRC_C:$(SRC_C_DIR)/%.c=$(OBJ_C_DIR)/%.o)

SRC_ASM_DIR=asm
SRC_ASM=$(wildcard asm/*)
OBJ_ASM_DIR=obj/asm
OBJ_ASM=$(SRC_ASM:$(SRC_ASM_DIR)/%.asm=$(OBJ_ASM_DIR)/%.o)

all: $(EXE)

$(EXE): $(OBJ_C_DIR) $(OBJ_C) $(OBJ_ASM_DIR) $(OBJ_ASM)
	$(LD) $(LD_FLAGS) $(OBJ_C) $(OBJ_ASM) -o $@

$(OBJ_C_DIR):
	$(call LOG,Creating folder $@)
	$(SILENT) mkdir -p $@

$(OBJ_C_DIR)/%.o: $(SRC_C_DIR)/%.c
	$(call LOG,Compiling C object $<)
	$(СС) $(CC_FLAGS) $< -o $@

$(OBJ_ASM_DIR):
	$(call LOG,Creating folder $@)
	$(SILENT) mkdir -p $@

$(OBJ_ASM_DIR)/%.o: $(SRC_ASM_DIR)/%.asm
	$(call LOG,Compiling ASM object $<)
	$(SILENT) $(ASM) $(ASM_FLAGS) $< -o $@

clean:
	$(call LOG,Deleting objects)
	$(SILENT) rm -rf $(OBJ_C_DIR)
	$(SILENT) rm -rf $(OBJ_ASM_DIR)

fclean: clean
	$(call LOG,Deleting executable)
	$(SILENT) rm -f $(EXE)

re: fclean all