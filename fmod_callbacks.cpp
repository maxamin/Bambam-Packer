// fmod_callbacks.cpp : implementation file
//

#include "stdafx.h"

typedef struct 
{
	int length;
	int pos;
	void *data;
} MEMFILE;


unsigned int memopen(char *name)
{
	MEMFILE *memfile;

	memfile = (MEMFILE *)calloc(sizeof(MEMFILE),1);

	{	// hey look some load from resource code!
		HRSRC		rec;
		HGLOBAL		handle;
		
		rec = FindResource(NULL, name, "RT_RCDATA");
		handle = LoadResource(NULL, rec);
		
		memfile->data = LockResource(handle);
		memfile->length = SizeofResource(NULL, rec);
		memfile->pos = 0;
	}

	return (unsigned int)memfile;
}

void memclose(unsigned int handle)
{
	MEMFILE *memfile = (MEMFILE *)handle;

	free(memfile);
}

int memread(void *buffer, int size, unsigned int handle)
{
	MEMFILE *memfile = (MEMFILE *)handle;

	if (memfile->pos + size >= memfile->length)
		size = memfile->length - memfile->pos;

	memcpy(buffer, (char *)memfile->data+memfile->pos, size);
	memfile->pos += size;
	
	return size;
}

void memseek(unsigned int handle, int pos, signed char mode)
{
	MEMFILE *memfile = (MEMFILE *)handle;

	if (mode == SEEK_SET) 
		memfile->pos = pos;
	else if (mode == SEEK_CUR) 
		memfile->pos += pos;
	else if (mode == SEEK_END)
		memfile->pos = memfile->length + pos;

	if (memfile->pos > memfile->length)
		memfile->pos = memfile->length;
}

int memtell(unsigned int handle)
{
	MEMFILE *memfile = (MEMFILE *)handle;

	return memfile->pos;
}