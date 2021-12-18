#pragma once
#include "config.hpp"

#define GRID_LEN 16
extern bool g_pause;

#define G_UNIT_SIZ 256
#define G_UNIT_INITSIZ 256
extern struct unit *g_unit;
extern int g_unit_size, g_unit_len;
extern unitid_t g_unit_id;

#define G_SELECTION_SIZE 8
extern unitid_t g_selection[G_SELECTION_SIZE];
extern int g_selection_len;
