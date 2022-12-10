LOG=$(info [rc4] $1)
SILENT=@

ASM=nasm
ASM_FLAGS=-f elf64

LIB=librc4.a

SRC_DIR=asm
SRC=$(wildcard asm/*)
OBJ_DIR=obj
OBJ=$(SRC:$(SRC_DIR)/%.asm=$(OBJ_DIR)/%.o)

all: $(LIB)

$(LIB): $(OBJ_DIR) $(OBJ)
	$(SILENT) ar -rc $@ $(OBJ)

$(OBJ_DIR):
	$(call LOG,Creating folder $@)
	$(SILENT) mkdir -p $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.asm
	$(call LOG,Compiling ASM object $<)
	$(SILENT) $(ASM) $(ASM_FLAGS) $< -o $@

clean:
	$(call LOG,Deleting objects)
	$(SILENT) rm -rf $(OBJ_C_DIR)
	$(SILENT) rm -rf $(OBJ_DIR)

fclean: clean
	$(call LOG,Deleting library)
	$(SILENT) rm -f $(LIB)

re: fclean all