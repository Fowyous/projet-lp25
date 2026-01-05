#ifndef UI_H
#define UI_H

#include "main.h"

typedef enum {
    LOCAL,
    REMOTE
} ServerType;


void run_tui(parameter_t *params);

#endif
