// Copyright (c) Athena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

#include "../common/cbasetypes.h"
#include "../common/malloc.h"
#include "../common/mmo.h"
#include "../common/socket.h"
#include "statusdb.h"
#include <string.h>

// status database
StatusDB* statuses = NULL;


void inter_status_init(void)
{
#ifdef TXT_ONLY
	statuses = status_db_txt();
#else
	statuses = status_db_sql();
#endif

	statuses->init(statuses);
}

void inter_status_final(void)
{
	statuses->destroy(statuses);
}

void inter_status_sync(void)
{
	statuses->sync(statuses);
}

bool inter_status_delete(int char_id)
{
	return statuses->remove(statuses, char_id);
}


int inter_status_tobuf(uint8* buf, size_t size, const struct scdata* sc)
{
	int i;
	size_t p;

	if( size < sc->count * sizeof(struct status_change_data) )
	{// not enough space
		//TODO: error message
		return 0;
	}

	for( i = 0, p = 0; i < sc->count && p < size; i++, p += sizeof(struct status_change_data) )
		memcpy(buf + p, &sc->data[i], sizeof(sc->data[i]));

	return p;
}

bool inter_status_frombuf(uint8* buf, size_t size, struct scdata* sc)
{
	int i;
	size_t p;
	int count = size / sizeof(struct status_change_data);
	
	if( size != count * sizeof(struct status_change_data) )
	{// size mismatch
		//TODO: error message
		return false;
	}

	sc->data = (struct status_change_data*)aMalloc(count*sizeof(struct status_change_data));
	sc->count = count;

	for( i = 0, p = 0; i < count && p < size; i++, p += sizeof(struct status_change_data) )
		memcpy(&sc->data[i], buf + p, sizeof(sc->data[i]));
	
	return true;
}


// Deliver status change data.
void mapif_status_load(int fd, int aid, int cid)
{
	struct scdata sc;

	if( !statuses->load_num(statuses, &sc, cid) )
		return; // no data

	// since status is only saved on final-save, erase data to prevent exploits
	//NOTE: if the mapserver crashes without a save handler in place, this data will be lost.
	statuses->remove(statuses, cid);

	WFIFOHEAD(fd, 14 + sc.count * sizeof(struct status_change_data));
	WFIFOW(fd,0) = 0x2b1d;
	WFIFOW(fd,2) = 14 + sc.count * sizeof(struct status_change_data);
	WFIFOL(fd,4) = aid;
	WFIFOL(fd,8) = cid;
	WFIFOW(fd,12) = sc.count;
	inter_status_tobuf(WFIFOP(fd,14), sc.count * sizeof(struct status_change_data), &sc);
	WFIFOSET(fd, WFIFOW(fd,2));

	aFree(sc.data);
}


void mapif_parse_StatusLoad(int fd)
{
	int aid = RFIFOL(fd,2);
	int cid = RFIFOL(fd,6);

	mapif_status_load(fd, aid, cid);
}

void mapif_parse_StatusSave(int fd)
{
	size_t size = RFIFOW(fd,2) - 14;
	int aid = RFIFOL(fd,4);
	int cid = RFIFOL(fd,8);
	int count = RFIFOW(fd,12);
	uint8* scbuf = RFIFOP(fd,14);

	struct scdata sc;
	sc.account_id = aid;
	sc.char_id = cid;
	if( !inter_status_frombuf(scbuf, size, &sc) )
	{// invalid data
		//TODO: error message
		return;
	}

	statuses->save(statuses, &sc);

	//TODO: deallocate sc->data
}


int inter_status_parse_frommap(int fd)
{
	switch(RFIFOW(fd,0))
	{
	case 0x30A0: mapif_parse_StatusLoad(fd); break;
	case 0x30A1: mapif_parse_StatusSave(fd); break;
	default:
		return 0;
	}
	return 1;
}
