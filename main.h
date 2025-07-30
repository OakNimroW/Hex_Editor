//===----------------------------------------------------------------------===//
///
/// \file
/// Definiciones y librerías globales al resto de módulos.
///
//===----------------------------------------------------------------------===//

#ifndef MAIN_H
#define MAIN_H

#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

// Constantes globales
#define HEX_WINDOW_PERCENT 80
#define CMD_WINDOW_PERCENT 20

/// Estados del programa
typedef enum {
  State_WaitingCommand, // Esperando un comando del usuario
  State_EditByte,       // Editando un byte en el archivo
  State_SaveFile,       // Guardando el archivo
  State_ShowHelp,       // Mostrando ayuda al usuario
  State_Exit            // Saliendo del editor
} editor_state_t;

// Estructura principal del editor
typedef struct {
  WINDOW *hex_window;           // Ventana para mostrar el contenido del archivo
  WINDOW *command_window;       // Ventana para mostrar comandos y mensajes
  editor_state_t current_state; // Estado actual del editor
  int running;                  // Indica si el editor está en ejecución
  int max_y, max_x;             // Dimensiones de la ventana principal
} hex_editor_t;

#endif