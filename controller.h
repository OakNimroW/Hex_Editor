#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "main.h"

//*                             ┌────────┐
//*                     ┌──────►│  EDIT  │
//*                     │ 'e'   │  BYTE  │
//*                     │ 'E'   └───┬────┘
//*                  ┌──┴────────┐  │      'q'
//*   ┌────────┐     │  WAITING  │◄─┘      'Q'  ┌────────┐
//*   │  INIT  ├────►│  COMMAND  ├─────────────►│  EXIT  │
//*   └────────┘     │           │◄─┐           └────────┘
//*                  └──┬────────┘  │
//*                     │ 'h'       │
//*                     │ 'H'   ┌───┴────┐
//*                     └──────►│  SHOW  │
//*                             │  HELP  │
//*                             └────────┘

// Funciones del controlador (MEF)
uint8_t controller_Init(hex_editor_t *editor);
void controller_Update(hex_editor_t *editor, uint8_t user_input);

#endif
