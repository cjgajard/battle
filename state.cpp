#include "state.hpp"

int g_pause = 0;
struct unit *g_unit;
int g_unit_size, g_unit_len;
unsigned long g_unit_id = 0;

unsigned long g_selection[G_SELECTION_SIZE] = {0};
int g_selection_len = 0;
