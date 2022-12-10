LOG=$(info [rc4-test] $1)
SILENT=@

RC4_DIR=../main
RC4_INCLUDE=$(RC4_DIR)/c/include
RC4_LIB=$(RC4_DIR)/librc4.a

СС=gcc
CC_FLAGS=-I $(RC4_INCLUDE) -I c/include -c

LD=gcc
LD_FLAGS=-no-pie

EXE=rc4-test

SRC_DIR=c/src
SRC=$(wildcard c/src/*)
OBJ_DIR=obj
OBJ=$(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: $(EXE)

$(EXE): $(RC4_LIB) $(OBJ_DIR) $(OBJ)
	$(SILENT) $(LD) $(LD_FLAGS) $(OBJ) $(RC4_LIB) -o $@

$(RC4_LIB):
	$(SILENT) make $(RC4_DIR)

$(OBJ_DIR):
	$(call LOG,Creating folder $@)
	$(SILENT) mkdir -p $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(call LOG,Compiling C object $<)
	$(SILENT) $(СС) $(CC_FLAGS) $< -o $@

clean:
	$(call LOG,Deleting objects)
	$(SILENT) rm -rf $(OBJ_DIR)

fclean: clean
	$(call LOG,Deleting executable)
	$(SILENT) rm -f $(EXE)

re: fclean all