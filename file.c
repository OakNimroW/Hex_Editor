//===----------------------------------------------------------------------===//
///
/// \file
/// Manejo de archivos binarios para el editor hexadecimal.
///
//===----------------------------------------------------------------------===//

#include "file.h"
#include "input.h"
#include <stdio.h>

//===----------------------------------------------------------------------===//
// Funciones para manejo de archivos
//===----------------------------------------------------------------------===//

int file_Load(hex_editor_t *editor) {
  if (!editor || !editor->filename) {
    if (editor) {
      input_ShowStatusMessage(editor, "[!] Error: No se especificó archivo");
    }
    return 1;
  }

  FILE *file = fopen(editor->filename, "rb");
  if (!file) {
    input_ShowStatusMessage(editor, "[!] Error: No se pudo abrir el archivo");
    return 1;
  }

  // Obtener tamaño del archivo
  fseek(file, 0, SEEK_END);
  editor->file_size = ftell(file);
  fseek(file, 0, SEEK_SET);

  // Allocar memoria para los datos
  editor->file_data = malloc(editor->file_size);
  if (!editor->file_data) {
    fclose(file);
    input_ShowStatusMessage(editor, "[!] Error: No se pudo asignar memoria");
    return 1;
  }

  // Leer archivo completo en memoria
  size_t bytes_read = fread(editor->file_data, 1, editor->file_size, file);
  fclose(file);

  if (bytes_read != editor->file_size) {
    free(editor->file_data);
    editor->file_data = NULL;
    input_ShowStatusMessage(editor,
                            "[!] Error: No se pudo leer el archivo completo");
    return 1;
  }

  input_ShowStatusMessage(editor, "[+] Archivo cargado correctamente");
  return 0;
}

void file_Cleanup(hex_editor_t *editor) {
  if (editor && editor->file_data) {
    free(editor->file_data);
    editor->file_data = NULL;
  }
  if (editor) {
    editor->file_size = 0;
  }
}
