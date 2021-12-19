#include "state.hpp"

bool g_pause = false;
unsigned int g_time = 0;

struct unit *g_unit;
int g_unit_size, g_unit_len;
unitid_t g_unit_id = 0;

unitid_t g_selection[G_SELECTION_SIZE] = {0};
int g_selection_len = 0;
