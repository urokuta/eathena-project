// Copyright (c) Athena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

#ifndef _INT_GUILD_H_
#define _INT_GUILD_H_

int inter_guild_init(void);
void inter_guild_final(void);
void inter_guild_sync(void);
int inter_guild_parse_frommap(int fd);
void inter_guild_delete(int guild_id);
void inter_guild_mapif_init(int fd);
int inter_guild_leave(int guild_id, int account_id, int char_id);
int inter_guild_sex_changed(int guild_id,int account_id,int char_id, int gender);

#ifndef TXT_ONLY
int inter_guild_CharOnline(int char_id, int guild_id);
int inter_guild_CharOffline(int char_id, int guild_id);
#endif

#endif /* _INT_GUILD_H_ */