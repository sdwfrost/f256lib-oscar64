/*
 * list.c - Doubly-linked list with merge sort
 * Ported from F256-FileManager CC65 version
 */

#include "list.h"
#include <stdlib.h>


/*****************************************************************************/
/*                       Private Function Prototypes                         */
/*****************************************************************************/

static WB2KList* List_MergeSortedList(WB2KList* list1, WB2KList* list2, bool (* compare_function)(void*, void*));
static void List_SplitList(WB2KList* source, WB2KList** front, WB2KList** back);
static void List_MergeSort(WB2KList** list_head, bool (* compare_function)(void*, void*));
static void List_RepairPrevLinks(WB2KList** list_head);


/*****************************************************************************/
/*                       Private Function Definitions                        */
/*****************************************************************************/

static WB2KList* List_MergeSortedList(WB2KList* list1, WB2KList* list2, bool (* compare_function)(void*, void*))
{
	WB2KList* result = NULL;

	if (list1 == NULL) return list2;
	if (list2 == NULL) return list1;

	if ((*compare_function)(list2->payload_, list1->payload_))
	{
		result = list1;
		result->next_item_ = List_MergeSortedList(list1->next_item_, list2, compare_function);
	}
	else
	{
		result = list2;
		result->next_item_ = List_MergeSortedList(list1, list2->next_item_, compare_function);
	}
	return result;
}


static void List_SplitList(WB2KList* source, WB2KList** front, WB2KList** back)
{
	WB2KList* ptr1;
	WB2KList* ptr2;

	ptr2 = source;
	ptr1 = source->next_item_;

	while (ptr1 != NULL)
	{
		ptr1 = ptr1->next_item_;
		if (ptr1 != NULL)
		{
			ptr2 = ptr2->next_item_;
			ptr1 = ptr1->next_item_;
		}
	}

	*front = source;
	*back = ptr2->next_item_;
	ptr2->next_item_ = NULL;
}


static void List_MergeSort(WB2KList** list_head, bool (* compare_function)(void*, void*))
{
	WB2KList* the_item = *list_head;
	WB2KList* ptr1;
	WB2KList* ptr2;

	if ((the_item == NULL) || (the_item->next_item_ == NULL))
		return;

	List_SplitList(the_item, &ptr1, &ptr2);
	List_MergeSort(&ptr1, compare_function);
	List_MergeSort(&ptr2, compare_function);
	*list_head = List_MergeSortedList(ptr1, ptr2, compare_function);
}


static void List_RepairPrevLinks(WB2KList** list_head)
{
	WB2KList* the_item;
	WB2KList* prev_item = *list_head;

	if (prev_item == NULL) return;
	if (prev_item->next_item_ == NULL) return;

	prev_item->prev_item_ = NULL;

	do
	{
		the_item = prev_item->next_item_;
		the_item->prev_item_ = prev_item;
		prev_item = the_item;
	} while (the_item->next_item_ != NULL);
}


/*****************************************************************************/
/*                        Public Function Definitions                        */
/*****************************************************************************/

WB2KList* List_NewItem(void* the_payload)
{
	WB2KList* the_item;

	the_item = (WB2KList*)malloc(sizeof(WB2KList));
	if (the_item == NULL) return NULL;

	the_item->next_item_ = NULL;
	the_item->prev_item_ = NULL;
	the_item->payload_ = the_payload;

	return the_item;
}


void List_Destroy(WB2KList** list_head)
{
	WB2KList* the_item;

	while (*list_head != NULL)
	{
		the_item = *list_head;
		*list_head = the_item->next_item_;
		free(the_item);
	}
}


void List_AddItem(WB2KList** list_head, WB2KList* the_item)
{
	if (*list_head == NULL)
	{
		the_item->prev_item_ = NULL;
		the_item->next_item_ = NULL;
	}
	else
	{
		the_item->next_item_ = *list_head;
		(*list_head)->prev_item_ = the_item;
		the_item->prev_item_ = NULL;
	}

	*list_head = the_item;
}


void List_Insert(WB2KList** head_item, WB2KList* the_item, WB2KList* previous_item)
{
	(void)head_item;
	(void)previous_item;
	// For now, just add as head
	List_AddItem(head_item, the_item);
}


void List_RemoveItem(WB2KList** list_head, WB2KList* the_item)
{
	if (*list_head == the_item)
	{
		*list_head = the_item->next_item_;
	}

	if (the_item->prev_item_ != NULL)
	{
		the_item->prev_item_->next_item_ = the_item->next_item_;
	}

	if (the_item->next_item_ != NULL)
	{
		the_item->next_item_->prev_item_ = the_item->prev_item_;
	}

	the_item->prev_item_ = NULL;
	the_item->next_item_ = NULL;
}


void List_InitMergeSort(WB2KList** list_head, bool (* compare_function)(void*, void*))
{
	List_MergeSort(list_head, compare_function);
	List_RepairPrevLinks(list_head);
}
