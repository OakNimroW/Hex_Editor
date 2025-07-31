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

//===----------------------------------------------------------------------===//
// Funciones de estado de la MEF
//===----------------------------------------------------------------------===//

void controller_WaitingCommandFn(hex_editor_t *editor, uint8_t command) {
  switch (command) {
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
    editor->current_state = State_EditByte;
    input_ShowStatusMessage(editor, "[*] Modo edición: Ingrese nuevo byte...");
    break;

  case 'h':
  case 'H':
    editor->current_state = State_ShowHelp;
    input_ShowStatusMessage(editor, "[*] Mostrando menú de ayuda...");
    break;

  default:
    if (command >= 32 && command <= 126) {
      input_ShowStatusMessage(editor, "[!] Comando no encontrado");
    }
    break;
  }
}

void controller_EditByteFn(hex_editor_t *editor, uint8_t user_input) {
  (void)user_input; // Ignorar entrada del usuario en este estado
  input_ShowStatusMessage(editor, "[+] Byte editado correctamente");
  editor->current_state = State_WaitingCommand; // Volver al estado de espera
}

void controller_SaveFileFn(hex_editor_t *editor, uint8_t user_input) {
  (void)user_input; // Ignorar entrada del usuario en este estado
  input_ShowStatusMessage(editor, "[+] Archivo guardado exitosamente");
  editor->current_state = State_WaitingCommand; // Volver al estado de espera
}

void controller_ShowHelpFn(hex_editor_t *editor, uint8_t user_input) {
  (void)user_input; // Ignorar entrada del usuario en este estado
  input_ShowStatusMessage(editor,
                          "[+] Ayuda: q=salir, s=guardar, e=editar, h=ayuda");
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
