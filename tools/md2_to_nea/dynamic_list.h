// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2008-2011, 2019, 2022, Antonio Niño Díaz

#ifndef _DYNAMICLIST_H_
#define _DYNAMICLIST_H_

typedef struct {
	void *previous;
	uint64_t data;
	void *next;
} DinamicList;

void DynamicListNew(DinamicList ** list);
int DynamicListNewElement(DinamicList * list);
void DynamicListElementSet(DinamicList * list, int index, uint64_t value);
uint64_t DynamicListElementGet(DinamicList * list, int index);
int DynamicListLenghtGet(DinamicList * list);
int DynamicListGetIndex(DinamicList * list, uint64_t value);
void DynamicListDelete(DinamicList * list);

#endif
