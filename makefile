SRC_DIR = src
RESOURCES_DIR = resources
TMP_DIR = $(RESOURCES_DIR)/tmp
OBJECT_DIR = $(RESOURCES_DIR)/object_files

SERVER_IMPL = $(SRC_DIR)/server.c
SERVER_OBJ = $(OBJECT_DIR)/server.o
SERVER_EXE = server

MEMORY_CONTROL_IMPL = $(SRC_DIR)/memory_control.c
MEMORY_CONTROL_OBJ = $(OBJECT_DIR)/memory_control.o

CONCURRENT_QUEUE_IMPL = $(SRC_DIR)/concurrent_queue.c
CONCURRENT_QUEUE_OBJ = $(OBJECT_DIR)/concurrent_queue.o

LINKED_LIST_IMPL = $(SRC_DIR)/linked_list.c
LINKED_LIST_OBJECT = $(OBJECT_DIR)/linked_list.o

# Cria o diretório dos arquivos, o git não permite commitar diretórios vazios.
build_resources_dir := $(shell mkdir -p $(RESOURCES_DIR))
build_tmp_dir := $(shell mkdir -p $(TMP_DIR))
build_object_dir := $(shell mkdir -p $(OBJECT_DIR))

build: $(SERVER_EXE)
	echo "success"

testar: $(SERVER_EXE)
	./server --server-port 8089 --mem-size 10000 --main-server --register-server "127.0.0.1::8090::10000"

# server
$(SERVER_EXE) : $(SERVER_OBJ) $(MEMORY_CONTROL_OBJ) $(CONCURRENT_QUEUE_OBJ) $(LINKED_LIST_OBJECT)
	 gcc -g3 $(SERVER_OBJ) $(MEMORY_CONTROL_OBJ) $(LINKED_LIST_OBJECT) $(CONCURRENT_QUEUE_OBJ) -o $(SERVER_EXE) -fopenmp -lm

$(SERVER_OBJ): $(SERVER_IMPL)
	gcc -g3 -c $(SERVER_IMPL) -fopenmp -o $(SERVER_OBJ)

# memory control
$(MEMORY_CONTROL_OBJ) : $(MEMORY_CONTROL_IMPL)
	gcc -g3 -c $(MEMORY_CONTROL_IMPL) -fopenmp -lm -o $(MEMORY_CONTROL_OBJ)

# concurrent_queue
$(CONCURRENT_QUEUE_OBJ) : $(CONCURRENT_QUEUE_IMPL)
	gcc -g3 -c $(CONCURRENT_QUEUE_IMPL) -fopenmp -o $(CONCURRENT_QUEUE_OBJ)

# linked list
$(LINKED_LIST_OBJECT) : $(LINKED_LIST_IMPL)
	gcc -g3 -c $(LINKED_LIST_IMPL) -fopenmp -o $(LINKED_LIST_OBJECT)

clean:
	rm -f $(OBJECT_DIR)/*.o
	rm -f $(TMP_DIR)/*.pid
	rm -f $(SERVER_EXE)
	rm -d -r $(RESOURCES_DIR)