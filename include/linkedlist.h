#ifndef LINKED_LIST_H_
#define LINKED_LIST_H_

#include "misc.h"

struct NodeData {
    int type;
    void *content;
};

struct LinkedNode {
    struct NodeData *data;
    struct LinkedNode *prev, *next;
};

struct LinkedList {
    int listSize;
    struct LinkedNode *head, *tail;
};

// Initialize a blank linked list
// Returns nothing
void initLinkedList(struct LinkedList *list);

// Destroy given linked list
// Returns nothing
void destroyLinkedList(struct LinkedList *list, void (*destroyDataFunc)(struct NodeData *));

// Create a new NodeData in memory
// Returns its pointer
struct NodeData * makeData(void *newContent, int newType);

// Add a new segment into the database
// Return the pointer of the added segment
struct LinkedNode * addNode(struct LinkedList *list, struct NodeData *newData);

// Edit data of a segment
// Returns nothing
void editNode(struct LinkedNode *node, struct NodeData *newData, void (*destroyDataFunc)(struct NodeData *));

// Find LinkedNode using given function
// Returns LinkedNode pointer
struct LinkedNode * findNode(struct LinkedList *list, const void *findData,
                            bool (*checkFunc)(const void *, const struct LinkedNode *));

// Move LinkedNode to list head
// Returns nothing
void moveToHead(struct LinkedList *list, struct LinkedNode *node);

// Move LinkedNode to list tail
// Returns nothing
void moveToTail(struct LinkedList *list, struct LinkedNode *node);

// Delete a segment
// Returns nothing
void deleteNode(struct LinkedList *list, struct LinkedNode *node,
                void (*destroyDataFunc)(struct NodeData *));

#endif
