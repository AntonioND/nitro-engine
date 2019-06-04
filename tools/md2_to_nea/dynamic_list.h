// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2008-2011, 2019, Antonio Niño Díaz

#ifndef _DYNAMICLIST_H_
#define _DYNAMICLIST_H_

typedef struct {
	void *previous;
	unsigned long long data;
	void *next;
} DinamicList;

void DynamicListNew(DinamicList ** list);
int DynamicListNewElement(DinamicList * list);
void DynamicListElementSet(DinamicList * list, int index,
			   unsigned long long value);
unsigned long long DynamicListElementGet(DinamicList * list, int index);
int DynamicListLenghtGet(DinamicList * list);
int DynamicListGetIndex(DinamicList * list, unsigned long long value);
void DynamicListDelete(DinamicList * list);

#endif
