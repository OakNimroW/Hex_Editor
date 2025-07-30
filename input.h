//===----------------------------------------------------------------------===//
///
/// \file
/// Definiciones para interactuar con la ventana de comandos.
///
//===----------------------------------------------------------------------===//

#ifndef INPUT_H
#define INPUT_H

#include "main.h"

// Funciones para manejo de la ventana de comandos
uint8_t input_Init(hex_editor_t *editor);
uint8_t input_GetCommand(hex_editor_t *editor);
void input_Refresh(hex_editor_t *editor);
void input_Cleanup(hex_editor_t *editor);

#endif // INPUT_H
