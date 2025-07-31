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

// Inicializar la ventana del editor hexadecimal
uint8_t output_Init(hex_editor_t *editor) {
  // Calcular dimensiones de la ventana hex
  uint16_t hex_height = (editor->max_y * HEX_WINDOW_PERCENT) / 100;

  // Crear ventana del editor hexadecimal
  editor->hex_window = newwin(hex_height, editor->max_x, 0, 0);
  if (editor->hex_window == NULL) {
    return -1;
  }

  // Configurar borde y título
  box(editor->hex_window, 0, 0);
  mvwprintw(editor->hex_window, 0, 2, " HEX EDITOR ");

  return 0;
}

// Mostrar contenido hexadecimal del archivo
void output_DisplayHexContent(hex_editor_t *editor) {
  if (!editor->file_data || editor->file_size == 0) {
    return;
  }

  // Limpiar área de contenido
  for (int i = 1; i < getmaxy(editor->hex_window) - 1; i++) {
    mvwhline(editor->hex_window, i, 1, ' ', getmaxx(editor->hex_window) - 2);
  }

  int window_height = getmaxy(editor->hex_window) - 2; // Restar bordes
  int bytes_per_line = 16;
  int max_lines = window_height - 1; // Dejar espacio para el borde superior

  size_t start_offset = editor->current_offset;
  size_t max_bytes = max_lines * bytes_per_line;

  int line = 1; // Comenzar después del borde superior

  for (size_t offset = start_offset;
       offset < editor->file_size && offset < start_offset + max_bytes;
       offset += bytes_per_line) {

    // Mostrar offset con 4 espacios después del ":"
    mvwprintw(editor->hex_window, line, 2, "%08zx:    ", offset);

    // Ajustar posición inicial del contenido hex
    int hex_col = 15; // 11 + 4 espacios
    for (int i = 0; i < bytes_per_line && (offset + i) < editor->file_size;
         i++) {
      uint8_t byte = editor->file_data[offset + i];

      if ((offset + i) == editor->cursor_position) {
        // Activar modo inverso para resaltar el cursor
        wattron(editor->hex_window, A_REVERSE);
      }

      mvwprintw(editor->hex_window, line, hex_col, "%02x", byte);

      if ((offset + i) == editor->cursor_position) {
        wattroff(editor->hex_window, A_REVERSE);
      }

      hex_col += 2;

      // Agregar espacio cada 2 bytes para mejor legibilidad
      if (i % 2 == 1) {
        mvwprintw(editor->hex_window, line, hex_col, " ");
        hex_col++;
      }
    }

    // Ajustar posición de la representación ASCII
    int ascii_col = 15 + (bytes_per_line * 2) + (bytes_per_line / 2) + 3;
    for (int i = 0; i < bytes_per_line && (offset + i) < editor->file_size;
         i++) {
      uint8_t byte = editor->file_data[offset + i];
      char ascii_char = (byte >= 32 && byte <= 126) ? byte : '.';

      if ((offset + i) == editor->cursor_position) {
        wattron(editor->hex_window, A_REVERSE);
      }

      mvwprintw(editor->hex_window, line, ascii_col + i, "%c", ascii_char);

      if ((offset + i) == editor->cursor_position) {
        wattroff(editor->hex_window, A_REVERSE);
      }
    }

    line++;

    // No mostrar más líneas si se llega al límite de la ventana
    if (line >= getmaxy(editor->hex_window) - 1) {
      break;
    }
  }

  // Redibujar borde y título
  box(editor->hex_window, 0, 0);
  mvwprintw(editor->hex_window, 0, 2, " HEX EDITOR - %s ",
            editor->filename ? editor->filename : "Sin archivo");
}

// Mostrar mensaje en la ventana hex-editor
void output_ShowMessage(hex_editor_t *editor, const char *message) {
  // Limpiar área de contenido
  for (uint16_t i = 1; i < getmaxy(editor->hex_window) - 1; i++) {
    mvwhline(editor->hex_window, i, 1, ' ', getmaxx(editor->hex_window) - 2);
  }

  // Mostrar mensaje
  mvwprintw(editor->hex_window, 2, 2, "%s", message);

  // Redibujar borde y título
  box(editor->hex_window, 0, 0);
  mvwprintw(editor->hex_window, 0, 2, " HEX EDITOR - %s ",
            editor->filename ? editor->filename : " Sin archivo ");
}

// Refrescar la ventana hex-editor
void output_Refresh(hex_editor_t *editor) { wrefresh(editor->hex_window); }

// Limpiar recursos de la ventana hex-editor
void output_Cleanup(hex_editor_t *editor) {
  if (editor->hex_window) {
    delwin(editor->hex_window);
    editor->hex_window = NULL;
  }
}
