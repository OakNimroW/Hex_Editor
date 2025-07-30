//===----------------------------------------------------------------------===//
///
/// \file
/// Utilidades para modificar la ventana de ejecicion de comandos.
///
//===----------------------------------------------------------------------===//

#include "input.h"

// Crear ventana
// * WINDOW *newwin(int nlines, int ncols, int begin_y, int begin_x);
// Agregar contorno - 0 para caracteres predeterminados
// * int box(WINDOW *win, chtype verch, chtype horch);
// Mostrar texto en ventana - Analogo a printf
// * int mvwprintw(WINDOW *win, int y, int x, const char *fmt, ...);
// Mover cursor
// * int wmove(WINDOW *win, int y, int x);
// Leer caracter de ventana - Analogo a scanf
// * int wgetch(WINDOW *win);
// Redibujar ventana
// * int wrefresh(WINDOW *win);

// Inicializar ventana de comandos
uint8_t input_Init(hex_editor_t *editor) {

  uint16_t cmd_height = (editor->max_y * CMD_WINDOW_PERCENT) / 100;
  uint16_t hex_height = editor->max_y - cmd_height;

  // Crear ventana de comandos
  editor->command_window = newwin(cmd_height, editor->max_x, hex_height, 0);
  if (editor->command_window == NULL) {
    return -1;
  }

  // Configurar borde y título
  box(editor->command_window, 0, 0);
  mvwprintw(editor->command_window, 0, 2, " COMMANDS ");

  // Mostrar instrucciones
  mvwprintw(editor->command_window, 1, 2,
            "Comandos: q=salir, s=guardar, e=editar, h=ayuda");
  mvwprintw(editor->command_window, 3, 2, "Ingresa un comando: ");

  return 0;
}

// Obtener comando del usuario
uint8_t input_GetCommand(hex_editor_t *editor) {
  // Posicionar cursor
  wmove(editor->command_window, 3, 22);
  wrefresh(editor->command_window);

  // Leer caracter de la ventana específica
  return wgetch(editor->command_window);
}

// Nueva función para mostrar un carácter en la línea de comandos
void input_ShowCommand(hex_editor_t *editor, uint8_t command) {
  if (command >= 32 && command <= 126) {
    // Escribir en ventana de comandos el caracter ingresado.
    mvwprintw(editor->command_window, 3, 22, "%c", command);
    wrefresh(editor->command_window);
  }
}

// Refrescar ventana de comandos
void input_Refresh(hex_editor_t *editor) {
  // Limpiar línea de comando
  mvwprintw(editor->command_window, 3, 2, "Ingresa un comando: ");

  // Redibujar borde y título
  box(editor->command_window, 0, 0);
  mvwprintw(editor->command_window, 0, 2, " COMMANDS ");

  wrefresh(editor->command_window);
}

// Limpiar recursos de la ventana de comandos
void input_Cleanup(hex_editor_t *editor) {
  if (editor->command_window) {
    delwin(editor->command_window);
    editor->command_window = NULL;
  }
}
