#ifndef GAME_H
#define GAME_H

int game_Init (void);
void game_Close (void);
void game_Update (void);
void game_Draw (double delta);

/* should OnEventName be moved to a event_ namespace? */
void game_OnKeydown (void *event);
void game_OnKeyup (void *event);
void game_OnClick (void *event);
void game_OnRelease (void *event);

/* extern struct unit *g_unit; */
/* extern int g_unit_size; */
/* extern int g_unit_len; */
/* extern unsigned long g_unit_id; */
#endif
