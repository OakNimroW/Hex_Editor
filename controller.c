//===----------------------------------------------------------------------===//
///
/// \file
/// Maquina de estados para controlar el programa.
///
//===----------------------------------------------------------------------===//

// Obtener numero de filas y columnas
// * void getmaxyx(const WINDOW *win, int *y, int *x);

#include "controller.h"
#include "file.h"
#include "input.h"
#include "output.h"

//===----------------------------------------------------------------------===//
// Funciones privadas de la MEF
//===----------------------------------------------------------------------===//

static void exitFn(hex_editor_t *editor) {
  editor->running = 0; // Terminar el bucle principal
  file_Cleanup(editor);
  input_Cleanup(editor);
  output_Cleanup(editor);
  endwin();
}

// Verificar si un carácter es un dígito hexadecimal
static int isHexChar(char c) {
  return ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') ||
          (c >= 'a' && c <= 'f'));
}

// Convertir carácter hex a valor numérico
static int hexCharToValue(char c) {
  if (c >= '0' && c <= '9')
    return c - '0';
  if (c >= 'A' && c <= 'F')
    return c - 'A' + 10;
  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;
  return -1; // Error
}

// Función para convertir string hex a byte
static int hexStringToByte(const char *hex_str) {
  if (strlen(hex_str) != 2)
    return -1;

  int high = hexCharToValue(hex_str[0]);
  int low = hexCharToValue(hex_str[1]);

  if (high == -1 || low == -1)
    return -1;

  return (high << 4) | low;
}

//===----------------------------------------------------------------------===//
// Funciones de navegación
//===----------------------------------------------------------------------===//

// Función para navegar hacia arriba (16 bytes hacia atrás)
static void controller_NavigateUp(hex_editor_t *editor) {
  if (!editor->file_data || editor->file_size == 0) {
    input_ShowStatusMessage(editor, "[!] No hay archivo cargado");
    return;
  }

  if (editor->cursor_position >= 16) {
    editor->cursor_position -= 16;

    // Ajustar offset de visualización si es necesario
    if (editor->cursor_position < editor->current_offset) {
      editor->current_offset = (editor->cursor_position / 16) * 16;
    }

    input_ShowStatusMessage(editor, "[*] Navegando hacia arriba");
  } else {
    editor->cursor_position = 0;
    editor->current_offset = 0;
    input_ShowStatusMessage(editor, "[!] Inicio del archivo");
  }
}

// Función para navegar hacia abajo (16 bytes hacia adelante)
static void controller_NavigateDown(hex_editor_t *editor) {
  if (!editor->file_data || editor->file_size == 0) {
    input_ShowStatusMessage(editor, "[!] No hay archivo cargado");
    return;
  }

  size_t new_position = editor->cursor_position + 16;
  if (new_position < editor->file_size) {
    editor->cursor_position = new_position;

    // Calcular límite inferior de la ventana
    int window_height = getmaxy(editor->hex_window) - 2;
    int visible_lines = window_height - 1;
    size_t window_bottom = editor->current_offset + (visible_lines * 16);

    // Ajustar offset si el cursor sale de la ventana
    if (editor->cursor_position >= window_bottom) {
      editor->current_offset += 16;
    }

    input_ShowStatusMessage(editor, "[*] Navegando hacia abajo");
  } else {
    input_ShowStatusMessage(editor, "[!] Final del archivo");
  }
}

// Función para navegar hacia la izquierda (1 byte hacia atrás)
static void controller_NavigateLeft(hex_editor_t *editor) {
  if (!editor->file_data || editor->file_size == 0) {
    input_ShowStatusMessage(editor, "[!] No hay archivo cargado");
    return;
  }

  if (editor->cursor_position > 0) {
    editor->cursor_position--;

    // Ajustar offset si es necesario
    if (editor->cursor_position < editor->current_offset) {
      editor->current_offset = (editor->cursor_position / 16) * 16;
    }

    input_ShowStatusMessage(editor, "[*] Navegando hacia la izquierda");
  } else {
    input_ShowStatusMessage(editor, "[!] Inicio del archivo");
  }
}

// Función para navegar hacia la derecha (1 byte hacia adelante)
static void controller_NavigateRight(hex_editor_t *editor) {
  if (!editor->file_data || editor->file_size == 0) {
    input_ShowStatusMessage(editor, "[!] No hay archivo cargado");
    return;
  }

  if (editor->cursor_position < editor->file_size - 1) {
    editor->cursor_position++;

    // Calcular límite inferior de la ventana
    int window_height = getmaxy(editor->hex_window) - 2;
    int visible_lines = window_height - 1;
    size_t window_bottom = editor->current_offset + (visible_lines * 16);

    // Ajustar offset si el cursor sale de la ventana
    if (editor->cursor_position >= window_bottom) {
      editor->current_offset += 16;
    }

    input_ShowStatusMessage(editor, "[*] Navegando hacia la derecha");
  } else {
    input_ShowStatusMessage(editor, "[!] Final del archivo");
  }
}

//===----------------------------------------------------------------------===//
// Funciones de estado de la MEF
//===----------------------------------------------------------------------===//

void controller_WaitingCommandFn(hex_editor_t *editor, uint8_t command) {
  switch ((unsigned)command) {
  case 'q':
  case 'Q':
    editor->current_state = State_Exit;
    input_ShowStatusMessage(editor, "[*] Saliendo del editor...");
    exitFn(editor);
    break;

  case 's':
  case 'S':
    editor->current_state = State_SaveFile;
    input_ShowStatusMessage(editor, "[*] Iniciando guardado de archivo...");
    break;

  case 'e':
  case 'E':
    if (!editor->file_data || editor->file_size == 0) {
      input_ShowStatusMessage(editor, "[!] No hay archivo cargado para editar");
    } else if (editor->cursor_position >= editor->file_size) {
      input_ShowStatusMessage(editor,
                              "[!] Posición del cursor fuera del archivo");
    } else {
      editor->current_state = State_EditByte;
      editor->edit_char_count = 0;
      editor->edit_buffer[0] = '\0';
      input_ShowStatusMessage(
          editor, "[*] Modo edición: Ingrese 2 dígitos hex (ej: FF, A0, 1B)");
    }
    break;

  case 'h':
  case 'H':
    editor->current_state = State_ShowHelp;
    input_ShowStatusMessage(editor, "[*] Mostrando menú de ayuda...");
    break;

  case 27: // ESC - inicio de secuencia de escape
  {
    // * Nota: Las flechas no se envían como un solo carácter, sino como una
    // *         secuencia de 3 caracteres:
    // * Flecha Arriba:       ESC + [ + A
    // * Flecha Abajo:        ESC + [ + B
    // * Flecha Derecha:      ESC + [ + C
    // * Flecha Izquierda:    ESC + [ + D

    // Leer el siguiente carácter sin bloquear
    nodelay(stdscr, TRUE);
    int next_char = getch();
    nodelay(stdscr, FALSE);

    if (next_char == '[') {
      // Leer el tercer carácter de la secuencia
      int arrow_char = getch();
      switch (arrow_char) {
      case 'A': // Flecha arriba
        controller_NavigateUp(editor);
        break;
      case 'B': // Flecha abajo
        controller_NavigateDown(editor);
        break;
      case 'C': // Flecha derecha
        controller_NavigateRight(editor);
        break;
      case 'D': // Flecha izquierda
        controller_NavigateLeft(editor);
        break;
      default:
        input_ShowStatusMessage(editor,
                                "[!] Secuencia de flecha no reconocida");
        break;
      }
    } else {
      // Solo ESC presionado
      input_ShowStatusMessage(editor, "[*] ESC presionado");
    }
  } break;

  default:
    if (command >= 32 && command <= 126) {
      input_ShowStatusMessage(editor, "[!] Comando no encontrado");
    }
    break;
  }
}

void controller_EditByteFn(hex_editor_t *editor, uint8_t user_input) {
  // Verificar que tenemos archivo cargado
  if (!editor->file_data || editor->file_size == 0) {
    input_ShowStatusMessage(editor, "[!] No hay archivo cargado");
    editor->current_state = State_WaitingCommand;
    return;
  }

  // Verificar posición del cursor
  if (editor->cursor_position >= editor->file_size) {
    input_ShowStatusMessage(editor,
                            "[!] Posición del cursor fuera del archivo");
    editor->current_state = State_WaitingCommand;
    return;
  }

  // Manejar ESC para cancelar edición
  if (user_input == 27 || user_input == 'q' ||
      user_input == 'Q') { // ESC o `q` para cancelar
    input_ShowStatusMessage(editor, "[*] Edición cancelada");
    editor->edit_char_count = 0;
    editor->edit_buffer[0] = '\0';
    editor->current_state = State_WaitingCommand;
    return;
  }

  // Manejar Backspace para borrar último carácter
  if (user_input == 8 || user_input == 127) { // Backspace o DEL
    if (editor->edit_char_count > 0) {
      editor->edit_char_count--;
      editor->edit_buffer[editor->edit_char_count] = '\0';

      if (editor->edit_char_count == 0) {
        input_ShowStatusMessage(
            editor, "[*] Modo edición: Ingrese 2 dígitos hex (ej: FF, A0, 1B)");
      } else {
        char msg[100] = {0};
        snprintf(msg, sizeof(msg),
                 "[*] Editando: %s_ (ingrese el segundo dígito)",
                 editor->edit_buffer);
        input_ShowStatusMessage(editor, msg);
      }
    }
    return;
  }

  // Verificar si es carácter hexadecimal válido
  if (!isHexChar(user_input)) {
    input_ShowStatusMessage(
        editor, "[!] Carácter inválido. Solo dígitos hex: 0-9, A-F, a-f");
    return;
  }

  // Agregar carácter al buffer
  editor->edit_buffer[editor->edit_char_count] = user_input;
  editor->edit_char_count++;
  editor->edit_buffer[editor->edit_char_count] = '\0';

  // Mostrar progreso
  if (editor->edit_char_count == 1) {
    char msg[100] = {0};
    snprintf(msg, sizeof(msg), "[*] Editando: %s_ (ingrese el segundo dígito)",
             editor->edit_buffer);
    input_ShowStatusMessage(editor, msg);
  } else if (editor->edit_char_count == 2) {
    // Tenemos 2 caracteres, procesar el byte
    int new_byte_value = hexStringToByte(editor->edit_buffer);

    if (new_byte_value == -1) {
      // Error de conversión (no debería pasar si isHexChar funciona bien)
      input_ShowStatusMessage(editor, "[!] Error interno: valor hex inválido");
      editor->edit_char_count = 0;
      editor->edit_buffer[0] = '\0';
    } else {
      // Actualizar el byte en el archivo
      uint8_t old_value = editor->file_data[editor->cursor_position];
      editor->file_data[editor->cursor_position] = (uint8_t)new_byte_value;

      // Mostrar confirmación
      char msg[100] = {0};
      snprintf(msg, sizeof(msg),
               "[+] Byte editado: 0x%02X -> 0x%02X en posición 0x%08zX",
               old_value, new_byte_value, editor->cursor_position);
      input_ShowStatusMessage(editor, msg);

      // Limpiar buffer y volver al modo normal
      editor->edit_char_count = 0;
      editor->edit_buffer[0] = '\0';
      editor->current_state = State_WaitingCommand;
    }
  }
}

void controller_SaveFileFn(hex_editor_t *editor, uint8_t user_input) {
  (void)user_input; // Ignorar entrada del usuario en este estado

  if (file_Save(editor) == 0) {
    input_ShowStatusMessage(editor, "[+] Archivo guardado exitosamente");
  }

  editor->current_state = State_WaitingCommand; // Volver al estado de espera
}

void controller_ShowHelpFn(hex_editor_t *editor, uint8_t user_input) {
  (void)user_input; // Ignorar entrada del usuario en este estado
  input_ShowStatusMessage(
      editor, "[+] Ayuda: Flechas=navegar, q=salir, s=guardar, e=editar byte");
  editor->current_state = State_WaitingCommand; // Volver al estado de espera
}
//===----------------------------------------------------------------------===//
// Funciones de control de la MEF
//===----------------------------------------------------------------------===//

static void (*StateFunctions[])(hex_editor_t *editor, uint8_t user_input) = {
    controller_WaitingCommandFn, // Esperando un comando del usuario
    controller_EditByteFn,       // Editando un byte en el archivo
    controller_SaveFileFn,       // Guardando el archivo
    controller_ShowHelpFn        // Mostrando ayuda al usuario
};

uint8_t controller_Init(hex_editor_t *editor) {
  // Verificar si el editor es nulo
  if (!editor) {
    return 1; // Error: editor nulo
  }

  // Inicializar estado del editor
  editor->current_state = State_WaitingCommand;
  editor->running = 1;
  editor->current_offset = 0;
  editor->cursor_position = 0;

  editor->edit_buffer[0] = '\0';
  editor->edit_char_count = 0;

  // Inicializar ncurses y ventanas
  initscr();            // Inicializar ncurses
  cbreak();             // Desactivar el modo de línea
  noecho();             // No mostrar la entrada del usuario
  keypad(stdscr, TRUE); // Habilitar teclas especiales

  getmaxyx(stdscr, editor->max_y, editor->max_x);

  // Inicializar ventanas
  if (input_Init(editor) != 0) {
    endwin();
    return -1;
  }

  if (output_Init(editor) != 0) {
    input_Cleanup(editor);
    endwin();
    return -1;
  }

  // Cargar archivo si se proporciona
  if (editor->filename) {
    if (file_Load(editor) != 0) {
      // No terminar el programa, solo mostrar error
      // El editor seguirá funcionando sin archivo
      input_ShowStatusMessage(editor, "[!] No se pudo cargar el archivo");
    }
  } else {
    input_ShowStatusMessage(editor,
                            "[*] Editor iniciado - No hay archivo cargado");
  }

  // Mostrar interfaz inicial
  output_DisplayHexContent(editor);
  output_Refresh(editor);
  input_Refresh(editor);

  return 0; // Éxito
}

void controller_Update(hex_editor_t *editor, uint8_t user_input) {
  // Verificar si el editor es nulo
  if (!editor || !editor->running) {
    return; // No hacer nada si el editor es nulo o no está en ejecución
  }

  // Llamar a la función del estado actual
  StateFunctions[editor->current_state](editor, user_input);

  // Actualizar la salidas (ventanas)
  output_DisplayHexContent(editor);
  output_Refresh(editor);
  input_Refresh(editor);

  // Mostrar el comando inmediatamente en línea de entrada
  input_ShowCommand(editor, user_input);
}
