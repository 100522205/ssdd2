#  Config
CC      = gcc
CFLAGS  = -Wall -Wextra -g -fPIC -pthread
# LDFLAGS: -L. busca librerías en la carpeta actual. -lrt es para colas de mensajes POSIX.
LDFLAGS = -L. -Wl,-rpath,.
LDLIBS  = -lrt

#  Nombres de archivos 
OBJ_DIR = .o
LIB_CLAVES  = libclaves.so
LIB_PROXY = libproxyclaves.so
SERVER    = servidor
CLIENT_DIST = cliente

#  Listas de Objetos
# Objetos para la lógica real
CLAVES_OBJS = $(OBJ_DIR)/claves.o $(OBJ_DIR)/list.o $(OBJ_DIR)/vector.o
# Objetos para el proxy
PROXY_OBJS = $(OBJ_DIR)/proxy-sock.o $(OBJ_DIR)/sock.o
# Objetos de los ejecutables
SERVER_OBJS = $(OBJ_DIR)/servidor-sock.o $(OBJ_DIR)/sock.o
CLIENT_OBJS = $(OBJ_DIR)/app-cliente.o $(OBJ_DIR)/tests.o

#reglas Principales 
all: $(LIB_CLAVES) $(LIB_PROXY) $(SERVER) $(CLIENT_DIST)

# 1. Librería Claves
$(LIB_CLAVES): $(CLAVES_OBJS)
	$(CC) -shared -o $@ $^ $(LDLIBS)

# 2. Librería Proxy
$(LIB_PROXY): $(PROXY_OBJS)
	$(CC) -shared -o $@ $^ $(LDLIBS)

# 3. Servidor
$(SERVER): $(SERVER_OBJS) $(LIB_CLAVES)
	$(CC) $(CFLAGS) -o $@ $(SERVER_OBJS) $(LDFLAGS) -lclaves $(LDLIBS)

# 5. Cliente Distribuido (con la lib proxy)
$(CLIENT_DIST): $(CLIENT_OBJS) $(LIB_PROXY)
	$(CC) $(CFLAGS) -o $@ $(CLIENT_OBJS) $(LDFLAGS) -lproxyclaves $(LDLIBS)

#regla Genérica para Objetos (Mantiene todo en .o/) 
$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@

clean:
	rm -rf $(OBJ_DIR) *.so $(SERVER) $(CLIENT_DIST) 

.PHONY: all clean