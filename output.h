//===----------------------------------------------------------------------===//
///
/// \file
/// Definiciones para interactuar con la ventana hex-editor.
///
//===----------------------------------------------------------------------===//

#ifndef OUTPUT_H
#define OUTPUT_H

#include "main.h"

// Funciones para manejo de la ventana hex-editor
uint8_t output_Init(hex_editor_t *editor);
void output_ShowMessage(hex_editor_t *editor, const char *message);
void output_DisplayHexContent(hex_editor_t *editor);
void output_Refresh(hex_editor_t *editor);
void output_Cleanup(hex_editor_t *editor);

#endif // OUTPUT_H
