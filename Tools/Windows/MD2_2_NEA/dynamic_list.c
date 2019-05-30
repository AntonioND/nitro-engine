
#include <malloc.h>
#include "dynamic_list.h"

void DynamicListNew(DinamicList ** list)
{
	*list = (DinamicList *) malloc(sizeof(DinamicList));
	(*list)->previous = NULL;
	(*list)->data = 0;
	(*list)->next = NULL;
}

int DynamicListNewElement(DinamicList * list)
{
	DinamicList *element_search = list;

	while (1) {
		if (element_search->next == NULL)
			break;

		element_search = (DinamicList *) element_search->next;
	}

	DinamicList *new_element;
	new_element = (DinamicList *) malloc(sizeof(DinamicList));
	element_search->next = new_element;

	new_element->previous = element_search;
	new_element->data = 0;
	new_element->next = NULL;

	return DynamicListLenghtGet(list) - 1;
}

void DynamicListElementSet(DinamicList * list, int index,
			   unsigned long long value)
{
	unsigned int count = 0;
	DinamicList *element_search = list;

	while (1) {
		if (count == index)
			break;
		element_search = (DinamicList *) element_search->next;
		count++;
	}

	element_search->data = value;
}

unsigned long long DynamicListElementGet(DinamicList * list, int index)
{
	unsigned int count = 0;
	DinamicList *element_search = list;

	while (1) {
		if (count == index)
			break;
		element_search = (DinamicList *) element_search->next;
		count++;
	}

	return element_search->data;
}

int DynamicListLenghtGet(DinamicList * list)
{
	unsigned int count = 0;
	DinamicList *element_search = list;

	while (1) {
		count++;
		element_search = (DinamicList *) element_search->next;
		if (element_search == NULL)
			break;
	}

	return count;
}

int DynamicListGetIndex(DinamicList * list, unsigned long long value)
{
	int count = 0;
	DinamicList *element_search = list;

	while (1) {
		if (element_search->data == value)
			return count;
		count++;
		element_search = (DinamicList *) element_search->next;
		if (element_search == NULL)
			break;	//No luck...
	}

	return -1;
}

void DynamicListDelete(DinamicList * list)
{
	DinamicList *element_search, *element_current = list;

	while (1) {
		element_search = (DinamicList *) element_current->next;
		free(element_current);
		if (element_search == NULL)
			break;
		element_current = element_search;
	}

	return;
}
