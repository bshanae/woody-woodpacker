LOG=$(info [lz77-test] $1)
SILENT=@

LZ77_DIR=../main
LZ77_INCLUDE=$(LZ77_DIR)/c/include
LZ77_LIB=$(LZ77_DIR)/liblz77.a

СС=gcc
CC_FLAGS=-I $(LZ77_INCLUDE) -I c/include -c

LD=gcc
LD_FLAGS=

EXE=lz77-test

SRC_DIR=c/src
SRC=$(wildcard c/src/*)
OBJ_DIR=obj
OBJ=$(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: $(EXE)

$(EXE): $(LZ77_LIB) $(OBJ_DIR) $(OBJ)
	$(SILENT) $(LD) $(LD_FLAGS) $(OBJ) $(LZ77_LIB) -o $@

$(LZ77_LIB):
	$(SILENT) make $(LZ77_DIR)

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