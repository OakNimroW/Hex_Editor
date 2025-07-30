CC = gcc                                    		# Compilador a usar
CFLAGS = -Wall -Wextra -std=c99             		# Flags de compilación
LIBS = -lncurses                            		# Bibliotecas a enlazar
TARGET = hex_editor                         		# Nombre del ejecutable final
SOURCES = main.c controller.c input.c output.c  	# Archivos fuente


all: $(TARGET)                             			# Regla por defecto, construye el TARGET

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET) $(LIBS)

clean:                                     			# Regla para limpiar archivos generados
	rm -f $(TARGET)

.PHONY: all clean                          			# Declara que all y clean no son archivos
