LOG=$(info [lz77] $1)
SILENT=@

СС=gcc
CC_FLAGS=-I c/include -c

LD=gcc
LD_FLAGS=-no-pie

LIB=liblz77.a

SRC_DIR=c/src
SRC=$(wildcard c/src/*)
OBJ_DIR=obj
OBJ=$(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: $(LIB)

$(LIB): $(OBJ_DIR) $(OBJ)
	$(SILENT) ar -rc $@ $(OBJ)

$(OBJ_DIR):
	$(call LOG,Creating folder $@)
	$(SILENT) mkdir -p $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(call LOG,Compiling C object $<)
	$(SILENT) $(CC) $(CC_FLAGS) $< -o $@

clean:
	$(call LOG,Deleting objects)
	$(SILENT) rm -rf $(OBJ_C_DIR)
	$(SILENT) rm -rf $(OBJ_DIR)

fclean: clean
	$(call LOG,Deleting library)
	$(SILENT) rm -f $(LIB)

re: fclean all