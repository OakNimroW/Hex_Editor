//===----------------------------------------------------------------------===//
///
/// \file
/// Utilidades para modificar la ventana de contenido de archivo.
///
//===----------------------------------------------------------------------===//

#include "output.h"

// Crear ventana
// * WINDOW *newwin(int nlines, int ncols, int begin_y, int begin_x);
// Agregar contorno - 0 para caracteres predeterminados
// * int box(WINDOW *win, chtype verch, chtype horch);
// Mostrar texto en ventana - Analogo a printf
// * int mvwprintw(WINDOW *win, int y, int x, const char *fmt, ...);
// Redibujar ventana
// * int wrefresh(WINDOW *win);
// Obtener numero de filas y columnas
// * int getmaxy(const WINDOW *win);
// * int getmaxx(const WINDOW *win);

// Inicializar ventana hex-editor
uint8_t output_Init(hex_editor_t *editor) {
  // calcular dimensiones de la ventana
  uint16_t hex_height = (editor->max_y * HEX_WINDOW_PERCENT) / 100;

  // Crear ventana hex-editor
  editor->hex_window = newwin(hex_height, editor->max_x, 0, 0);
  if (editor->hex_window == NULL) {
    return -1;
  }

  // Configurar borde y titulo
  box(editor->hex_window, 0, 0);
  mvwprintw(editor->hex_window, 0, 2, " HEX EDITOR ");

  return 0;
}

// Mostrar mensaje en ventana hex-editor
void output_ShowMessage(hex_editor_t *editor, const char *message) {
  // Limpiar area de contenido
  for (uint16_t i = 1; i < getmaxy(editor->hex_window) - 1; i++) {
    mvwhline(editor->hex_window, i, 1, ' ', getmaxx(editor->hex_window) - 2);
  }

  // Mostrar mensaje
  mvwprintw(editor->hex_window, 1, 2, "%s", message);

  // Redibujar borde y titulo
  box(editor->hex_window, 0, 0);
  mvwprintw(editor->hex_window, 0, 2, " HEX EDITOR ");
}

// Refrescar ventana hex-editor
void output_Refresh(hex_editor_t *editor) { wrefresh(editor->hex_window); }

// Limpiar ventana hex-editor
void output_Cleanup(hex_editor_t *editor) {
  if (editor->hex_window != NULL) {
    delwin(editor->hex_window);
    editor->hex_window = NULL;
  }
}