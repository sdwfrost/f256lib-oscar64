/*
 * list.h - Doubly-linked list with merge sort
 * Ported from F256-FileManager CC65 version
 */

#ifndef LIST_H_
#define LIST_H_

#include "f256lib.h"

#pragma compile("list.c")


/*****************************************************************************/
/*                                 Structs                                   */
/*****************************************************************************/

typedef struct WB2KList WB2KList;

struct WB2KList
{
	WB2KList*	next_item_;
	WB2KList*	prev_item_;
	void*		payload_;
};


/*****************************************************************************/
/*                       Public Function Prototypes                          */
/*****************************************************************************/

// generates a new list item. Does not add the list item to a list.
WB2KList* List_NewItem(void* the_payload);

// destructor - frees list nodes but NOT payloads
void List_Destroy(WB2KList** head_item);

// adds a new list item as the head of the list
void List_AddItem(WB2KList** head_item, WB2KList* the_item);

// adds a new list item before the list_item passed
void List_Insert(WB2KList** head_item, WB2KList* the_item, WB2KList* previous_item);

// removes the specified item from the list (without destroying the list item)
void List_RemoveItem(WB2KList** head_item, WB2KList* the_item);

// Merge Sort. pass a function that compares payloads, returns true if thing 1 > thing 2
void List_InitMergeSort(WB2KList** list_head, bool (* compare_function)(void*, void*));


#endif /* LIST_H_ */
