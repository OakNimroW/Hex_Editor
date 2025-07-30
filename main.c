//===----------------------------------------------------------------------===//
///
/// \file
/// Inicio del programa. Contiene llamados a inicialización y bucle principal.
///
//===----------------------------------------------------------------------===//

#include "main.h"
#include "controller.h"
#include "input.h"
#include "output.h"

int main(void) {
  hex_editor_t editor = {0};

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
      // Leer input del usuario.
      ch = input_GetCommand(&editor);
    }

    // Procesar la entrada del usuario
    controller_Update(&editor, ch);
  }

  controller_Update(&editor, ch);

  printf("\n[+] Editor cerrado correctamente.\n\n");
  return EXIT_SUCCESS;
}
