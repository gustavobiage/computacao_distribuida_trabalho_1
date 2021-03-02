SRC_DIR = src
RESOURCES_DIR = resources
TMP_DIR = $(RESOURCES_DIR)/tmp
OBJECT_DIR = $(RESOURCES_DIR)/object_files

SERVER_IMPL = $(SRC_DIR)/server.c
SERVER_OBJ = $(OBJECT_DIR)/server.o
SERVER_EXE = server

MEMORY_CONTROL_IMPL = $(SRC_DIR)/memory_control.c
MEMORY_CONTROL_OBJ = $(OBJECT_DIR)/memory_control.o

# Cria o diretório dos arquivos, o git não permite commitar diretórios vazios.
build_resources_dir := $(shell mkdir -p $(RESOURCES_DIR))
build_tmp_dir := $(shell mkdir -p $(TMP_DIR))
build_object_dir := $(shell mkdir -p $(OBJECT_DIR))

build: $(SERVER_EXE)
	echo "success"
# server
$(SERVER_EXE) : $(SERVER_OBJ) $(MEMORY_CONTROL_OBJ)
	 gcc -g3 $(SERVER_OBJ) $(MEMORY_CONTROL_OBJ) -o $(SERVER_EXE) -fopenmp -lm
$(SERVER_OBJ): $(SERVER_IMPL)
	gcc -g3 -c $(SERVER_IMPL) -fopenmp -o $(SERVER_OBJ)
# memory control
$(MEMORY_CONTROL_OBJ) : $(MEMORY_CONTROL_IMPL)
	gcc -g3 -c $(MEMORY_CONTROL_IMPL) -fopenmp -lm -o $(MEMORY_CONTROL_OBJ)