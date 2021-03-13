SRC_DIR = src
RESOURCES_DIR = resources
TMP_DIR = $(RESOURCES_DIR)/tmp
OBJECT_DIR = $(RESOURCES_DIR)/object_files
TESTS_DIR = $(SRC_DIR)/tests

# Implementações
SERVER_IMPL = $(SRC_DIR)/server.c
CLIENT_IMPL = $(SRC_DIR)/client.c
MEMORY_CONTROL_IMPL = $(SRC_DIR)/memory_control.c
CONCURRENT_QUEUE_IMPL = $(SRC_DIR)/concurrent_queue.c
LINKED_LIST_IMPL = $(SRC_DIR)/linked_list.c
VECTOR_IMPL = $(SRC_DIR)/vector.c
FIB_CLIENT_IMPL = $(TESTS_DIR)/fib.c
LOGGER_IMPL = $(SRC_DIR)/logger.c

# Arquivos não linkados
SERVER_OBJ = $(OBJECT_DIR)/server.o
CLIENT_OBJ = $(OBJECT_DIR)/client.o
MEMORY_CONTROL_OBJ = $(OBJECT_DIR)/memory_control.o
CONCURRENT_QUEUE_OBJ = $(OBJECT_DIR)/concurrent_queue.o
LINKED_LIST_OBJECT = $(OBJECT_DIR)/linked_list.o
VECTOR_OBJ = $(OBJECT_DIR)/vector.o
FIB_CLIENT_OBJ = $(OBJECT_DIR)/fib.o
LOGGER_OBJ = $(OBJECT_DIR)/logger.o

# Executáveis
SERVER_EXE = server
FIB_CLIENT_EXE = fib
SERVER_PID_FILE = $(TMP_DIR)/server.pid
LOGGER_EXE = logger

# Cria o diretório dos arquivos, o git não permite commitar diretórios vazios.
build_resources_dir := $(shell mkdir -p $(RESOURCES_DIR))
build_tmp_dir := $(shell mkdir -p $(TMP_DIR))
build_object_dir := $(shell mkdir -p $(OBJECT_DIR))

build: $(SERVER_EXE) $(LOGGER_EXE) $(FIB_CLIENT_EXE)
	@echo "success"

testar: build
 	## ./server --server-port 8089 --mem-size 10000 --main-server --register-server "127.0.0.1::8090::10000"
	@./$(SERVER_EXE) --server-port 8089 --mem-size 10000 --main-server &
	@./$(FIB_CLIENT_EXE) 127.0.0.1 8089 10
	@echo "finalizando servidor"
	@killall $(SERVER_EXE)
	@killall $(LOGGER_EXE)

# server
$(SERVER_EXE) : $(LOGGER_EXE) $(SERVER_OBJ) $(MEMORY_CONTROL_OBJ) $(CONCURRENT_QUEUE_OBJ) $(LINKED_LIST_OBJECT)
	 gcc -g3 $(SERVER_OBJ) $(MEMORY_CONTROL_OBJ) $(LINKED_LIST_OBJECT) $(CONCURRENT_QUEUE_OBJ) -o $(SERVER_EXE) -fopenmp -lm

$(SERVER_OBJ): $(SERVER_IMPL)
	gcc -g3 -c $(SERVER_IMPL) -fopenmp -o $(SERVER_OBJ)

# logger
$(LOGGER_EXE) : $(LOGGER_OBJ) $(CLIENT_OBJ) $(VECTOR_OBJ) $(LINKED_LIST_OBJECT)
	gcc -g3 $(LOGGER_OBJ) $(CLIENT_OBJ) $(VECTOR_OBJ) $(LINKED_LIST_OBJECT) -fopenmp -lm -o $(LOGGER_EXE)

$(LOGGER_OBJ) : $(LOGGER_IMPL)
	gcc -g3 -c $(LOGGER_IMPL) -fopenmp -lm -o $(LOGGER_OBJ)

# memory control
$(MEMORY_CONTROL_OBJ) : $(MEMORY_CONTROL_IMPL)
	gcc -g3 -c $(MEMORY_CONTROL_IMPL) -fopenmp -lm -o $(MEMORY_CONTROL_OBJ)

# concurrent_queue
$(CONCURRENT_QUEUE_OBJ) : $(CONCURRENT_QUEUE_IMPL)
	gcc -g3 -c $(CONCURRENT_QUEUE_IMPL) -fopenmp -o $(CONCURRENT_QUEUE_OBJ)

# linked list
$(LINKED_LIST_OBJECT) : $(LINKED_LIST_IMPL)
	gcc -g3 -c $(LINKED_LIST_IMPL) -fopenmp -o $(LINKED_LIST_OBJECT)

# client
$(CLIENT_OBJ) : $(CLIENT_IMPL)
	gcc -g3 $(CLIENT_IMPL) -c -fopenmp -o $(CLIENT_OBJ)

# vector
$(VECTOR_OBJ) : $(VECTOR_IMPL)
	gcc -g3 -c $(VECTOR_IMPL) -fopenmp -o $(VECTOR_OBJ)

# fibonacci client test
$(FIB_CLIENT_OBJ) : $(FIB_CLIENT_IMPL)
	gcc -g3 -c $(FIB_CLIENT_IMPL) -fopenmp -o $(FIB_CLIENT_OBJ)
$(FIB_CLIENT_EXE) : $(FIB_CLIENT_OBJ) $(CLIENT_OBJ) $(VECTOR_OBJ)
	gcc -g3 $(FIB_CLIENT_OBJ) $(CLIENT_OBJ) $(VECTOR_OBJ) -fopenmp -o $(FIB_CLIENT_EXE)

clean:
	rm -f $(OBJECT_DIR)/*.o
	rm -f $(TMP_DIR)/*.pid
	rm -f $(RESOURCES_DIR)/*.log
	rm -f $(SERVER_EXE)
	rm -f $(FIB_CLIENT_EXE)
	rm -f $(LOGGER_EXE)
	rm -d -r $(RESOURCES_DIR)