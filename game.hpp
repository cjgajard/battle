#pragma once
int game_Init (void);
void game_Close (void);
void game_Update (void);
void game_Draw (double delta);

/* should OnEventName be moved to a event_ namespace? */
void game_OnKeydown (void *event);
void game_OnKeyup (void *event);
void game_OnClick (void *event);
void game_OnRelease (void *event);
