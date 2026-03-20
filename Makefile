#  Config
CC      = gcc
CFLAGS  = -Wall -Wextra -g -fPIC -pthread
# LDFLAGS: -L. busca librerías en la carpeta actual. -lrt es para colas de mensajes POSIX.
LDFLAGS = -L. -Wl,-rpath,.
LDLIBS  = -lrt

#  Nombres de archivos 
OBJ_DIR = .o
LIB_MONO  = libclaves.so
LIB_PROXY = libproxyclaves.so
SERVER    = servidor_mq
CLIENT_MONO = app_cliente_mono
CLIENT_DIST = cliente_mq

#  Listas de Objetos
# Objetos para la lógica real (BD)
DB_OBJS = $(OBJ_DIR)/claves.o $(OBJ_DIR)/list.o $(OBJ_DIR)/vector.o
# Objetos para el proxy
PROXY_OBJS = $(OBJ_DIR)/proxy-mq.o
# Objetos de los ejecutables
SERVER_OBJS = $(OBJ_DIR)/servidor-mq.o
CLIENT_OBJS = $(OBJ_DIR)/app-cliente.o $(OBJ_DIR)/tests.o

#reglas Principales 
all: $(LIB_MONO) $(LIB_PROXY) $(SERVER) $(CLIENT_MONO) $(CLIENT_DIST)

# 1. Librería Monolítica (Contiene la lógica de la lista)
$(LIB_MONO): $(DB_OBJS)
	$(CC) -shared -o $@ $^

# 2. Librería Proxy (Contiene las llamadas a mq_send/mq_receive)
$(LIB_PROXY): $(PROXY_OBJS)
	$(CC) -shared -o $@ $^ $(LDLIBS)

# 3. Servidor (Usa la lógica de la BD directamente y mqueue)
$(SERVER): $(SERVER_OBJS) $(DB_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

# 4. Cliente Monolítico (Enlazado con la lib real)
$(CLIENT_MONO): $(CLIENT_OBJS) $(LIB_MONO)
	$(CC) $(CFLAGS) -o $@ $(CLIENT_OBJS) $(LDFLAGS) -lclaves

# 5. Cliente Distribuido (con la lib proxy)
$(CLIENT_DIST): $(CLIENT_OBJS) $(LIB_PROXY)
	$(CC) $(CFLAGS) -o $@ $(CLIENT_OBJS) $(LDFLAGS) -lproxyclaves $(LDLIBS)

#regla Genérica para Objetos (Mantiene todo en .o/) 
$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@

clean:
	rm -rf $(OBJ_DIR) *.so $(SERVER) $(CLIENT_MONO) $(CLIENT_DIST)

.PHONY: all clean