// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2008-2011, 2019, 2022 Antonio Niño Díaz

#include <stdint.h>
#include <stdlib.h>

#include "dynamic_list.h"

void DynamicListNew(DynamicList **list)
{
    *list = (DynamicList *)malloc(sizeof(DynamicList));
    (*list)->previous = NULL;
    (*list)->data = 0;
    (*list)->next = NULL;
}

int DynamicListNewElement(DynamicList *list)
{
    DynamicList *element_search = list;

    while (1)
    {
        if (element_search->next == NULL)
            break;

        element_search = (DynamicList *)element_search->next;
    }

    DynamicList *new_element;
    new_element = (DynamicList *)malloc(sizeof(DynamicList));
    element_search->next = new_element;

    new_element->previous = element_search;
    new_element->data = 0;
    new_element->next = NULL;

    return DynamicListLenghtGet(list) - 1;
}

void DynamicListElementSet(DynamicList *list, int index, uint64_t value)
{
    int count = 0;
    DynamicList *element_search = list;

    while (1)
    {
        if (count == index)
            break;
        element_search = (DynamicList *)element_search->next;
        count++;
    }

    element_search->data = value;
}

uint64_t DynamicListElementGet(DynamicList *list, int index)
{
    int count = 0;
    DynamicList *element_search = list;

    while (1)
    {
        if (count == index)
            break;
        element_search = (DynamicList *)element_search->next;
        count++;
    }

    return element_search->data;
}

int DynamicListLenghtGet(DynamicList *list)
{
    int count = 0;
    DynamicList *element_search = list;

    while (1)
    {
        count++;
        element_search = (DynamicList *)element_search->next;
        if (element_search == NULL)
            break;
    }

    return count;
}

int DynamicListGetIndex(DynamicList *list, uint64_t value)
{
    int count = 0;
    DynamicList *element_search = list;

    while (1)
    {
        if (element_search->data == value)
            return count;
        count++;
        element_search = (DynamicList *)element_search->next;
        if (element_search == NULL)
            break; //No luck...
    }

    return -1;
}

void DynamicListDelete(DynamicList *list)
{
    DynamicList *element_search;
    DynamicList *element_current = list;

    while (1)
    {
        element_search = (DynamicList *)element_current->next;
        free(element_current);
        if (element_search == NULL)
            break;
        element_current = element_search;
    }

    return;
}
