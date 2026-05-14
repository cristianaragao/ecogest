CC     = gcc
CFLAGS = -Wall -Wextra -std=c11
SRC    = src/main.c src/auth.c src/cliente.c src/funcionario.c \
         src/residuo.c src/relatorio.c src/utils.c
OUT    = ecogest

all:
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

clean:
	rm -f $(OUT) *.o

run:
	./$(OUT)
