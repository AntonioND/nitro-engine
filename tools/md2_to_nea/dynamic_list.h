// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2008-2011, 2019, 2022, Antonio Niño Díaz

#ifndef _DYNAMICLIST_H_
#define _DYNAMICLIST_H_

typedef struct {
    void *previous;
    uint64_t data;
    void *next;
} DynamicList;

void DynamicListNew(DynamicList **list);
int DynamicListNewElement(DynamicList *list);
void DynamicListElementSet(DynamicList *list, int index, uint64_t value);
uint64_t DynamicListElementGet(DynamicList *list, int index);
int DynamicListLenghtGet(DynamicList *list);
int DynamicListGetIndex(DynamicList *list, uint64_t value);
void DynamicListDelete(DynamicList *list);

#endif
