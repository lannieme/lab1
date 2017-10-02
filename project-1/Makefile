CC = gcc
CFLAGS = -Wall -g
SRC_DIR = src
OBJ_DIR = obj

_SERVER_OBJS = echo_server.o logger.o y.tab.o lex.yy.o
SERVER_OBJS = $(patsubst %,$(OBJ_DIR)/%,$(_SERVER_OBJS))
_LISOD_OBJS = lisod.o logger.o y.tab.o lex.yy.o parse.o response.o request.o request_handler.o
LISOD_OBJS = $(patsubst %,$(OBJ_DIR)/%,$(_LISOD_OBJS))

default: lisod echo_client

lisod: $(OBJ_DIR) $(LISOD_OBJS)
	$(CC) $(CFLAGS) -o lisod $(LISOD_OBJS)

echo_server: $(OBJ_DIR) $(SERVER_OBJS)
	$(CC) $(CFLAGS) -o echo_server $(SERVER_OBJS)

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

$(OBJ_DIR)/echo_server.o: $(SRC_DIR)/echo_server.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/lisod.o: $(SRC_DIR)/lisod.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/request.o: $(SRC_DIR)/request.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/response.o: $(SRC_DIR)/response.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/parse.o: $(SRC_DIR)/parser/parse.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/logger.o: $(SRC_DIR)/logger/logger.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/request_handler.o: $(SRC_DIR)/request_handler/request_handler.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(SRC_DIR)/parser/y.tab.c: $(SRC_DIR)/parser/parser.y
	yacc -d $^ -o $@

$(OBJ_DIR)/y.tab.o: $(SRC_DIR)/parser/y.tab.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(SRC_DIR)/parser/lex.yy.c: $(SRC_DIR)/parser/lexer.l
	flex -o $@ $^

$(OBJ_DIR)/lex.yy.o: $(SRC_DIR)/parser/lex.yy.c
	$(CC) $(CFLAGS) -c -o $@ $<

echo_client:
	$(CC) $(CFLAGS) $(SRC_DIR)/echo_client.c -o echo_client

clean:
	rm -rf lisod echo_server echo_client $(OBJ_DIR)
	rm -rf $(SRC_DIR)/parser/lex.yy.c $(SRC_DIR)/parser/y.tab.c $(SRC_DIR)/parser/y.tab.h
