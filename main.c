//===----------------------------------------------------------------------===//
///
/// \file
/// Inicio del programa. Contiene llamados a inicialización y bucle principal.
///
//===----------------------------------------------------------------------===//

#include "main.h"
#include "controller.h"
#include "file.h"
#include "input.h"
#include "output.h"

int main(int argc, char *argv[]) {
  hex_editor_t editor = {0};

  // El archivo es opcional
  if (argc > 2) {
    fprintf(stderr, "[!] Uso: %s [archivo_opcional]\n", argv[0]);
    return EXIT_FAILURE;
  }

  if (argc == 2) {
    size_t filename_length = strlen(argv[1]);
    if (filename_length > 144) {
      fprintf(stderr,
              "[!] Error: El nombre del archivo supera los 144 caracteres (%zu "
              "caracteres).\n",
              filename_length);
      return EXIT_FAILURE;
    }
    editor.filename = argv[1];
  }

  // Inicialización del editor
  if (controller_Init(&editor) != 0) {
    fprintf(stderr, "[!] Error: No se pudo inicializar el editor.\n");
    return EXIT_FAILURE;
  }

  // Bucle principal
  uint32_t ch;
  while (editor.running) {

    if (editor.current_state != State_SaveFile &&
        editor.current_state != State_ShowHelp &&
        editor.current_state != State_Exit) {
      ch = input_GetCommand(&editor);
    }

    // Procesar la entrada del usuario
    controller_Update(&editor, ch);
  }

  printf("\n[+] Editor cerrado correctamente.\n\n");
  return EXIT_SUCCESS;
}