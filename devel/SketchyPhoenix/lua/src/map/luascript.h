// Copyright (c) Athena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

#ifndef _LUASCRIPT_H_
#define _LUASCRIPT_H_

#include <lua.h>

//Lua functions
struct LuaCommandInfo {
	const char *command;
	lua_CFunction f;
};

extern struct m_drops mine_db[100];
extern struct m_points mine_materials[300];

int lua_eventtimer[MAX_EVENTTIMER];
unsigned short lua_eventcount;

lua_State *L;

int menu_countoptions(const char* str, int max_count, int* total);
int do_init_luascript(void);
void script_buildin_commands_lua(void);
void script_run_function(const char *name,int char_id,const char *format,...);
void script_run_chunk(const char *chunk,int char_id);
void script_resume(struct map_session_data *sd,const char *format,...);
void do_final_luascript(void);

extern enum { L_NRUN,L_CLOSE,L_NEXT,L_INPUT,L_MENU, L_END };

#endif /* _LUASCRIPT_H_ */