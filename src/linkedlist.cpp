#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>

#include "linkedlist.h"

void initLinkedList(struct LinkedList *list) {
    assert(list != NULL);
    list -> listSize = 0;
    list -> head = NULL;
    list -> tail = NULL;
}

void destroyLinkedList(struct LinkedList *list, void (*destroyDataFunc)(struct NodeData *)) {
    assert(list != NULL);
    struct LinkedNode *cntNode = list -> head;
    while (cntNode != NULL) {
        struct LinkedNode *delNode = cntNode;
        cntNode = cntNode -> next;
        deleteNode(list, delNode, destroyDataFunc);
    }
    free(list);
    list = NULL;
}

struct NodeData * makeData(void *newContent, int newType) {
    assert(newContent != NULL);
    struct NodeData *data = (struct NodeData *)malloc(sizeof(struct NodeData));
    data -> content = newContent;
    data -> type = newType;
    return data;
}

// Complexity: O(1)
struct LinkedNode * addNodeAtHead(struct LinkedList *list, struct NodeData *newData) {
    assert(list != NULL && newData != NULL);
    errno = 0;
    // Create new segment
    struct LinkedNode *newNode = (struct LinkedNode *)malloc(sizeof(struct LinkedNode));
    if (newNode == NULL) {
        free(newNode);
        perror("addNode");
        return NULL;
    }

    newNode -> data = newData;
    newNode -> prev = NULL;
    newNode -> next = list -> head;

    // Maintain list
    if (list -> tail == NULL) {
        list -> tail = newNode;
    }
    if (list -> head != NULL) {
        list -> head -> prev = newNode;
    }

    list -> head = newNode;
    list -> listSize++;

    return newNode;
}

struct LinkedNode * addNodeAtTail(struct LinkedList *list, struct NodeData *newData) {
    assert(list != NULL && newData != NULL);
    errno = 0;
    // Create new segment
    struct LinkedNode *newNode = (struct LinkedNode *)malloc(sizeof(struct LinkedNode));
    if (newNode == NULL) {
        free(newNode);
        perror("addNode");
        return NULL;
    }

    newNode -> data = newData;
    newNode -> prev = list -> tail;
    newNode -> next = NULL;

    // Maintain list
    if (list -> head == NULL) {
        list -> head = newNode;
    }
    if (list -> tail != NULL) {
        list -> tail -> next = newNode;
    }

    list -> tail = newNode;
    list -> listSize++;

    return newNode;
}

// Complexity: O(1)
void editNode(struct LinkedNode *node, struct NodeData *newData, void (*destroyDataFunc)(struct NodeData *)) {
    assert(node != NULL && newData != NULL);
    destroyDataFunc(node -> data);
    node -> data = newData;
}

struct LinkedNode * findNode(struct LinkedList *list, const void *findData,
                            bool (*checkFunc)(const void *, const struct LinkedNode *)) {
    assert(list != NULL && findData != NULL && checkFunc != NULL);
    struct LinkedNode *cntNode = list -> tail;
    while (cntNode != NULL) {
        if (checkFunc(findData, cntNode)) {
            return cntNode;
        }
        cntNode = cntNode -> prev;
    }
    return NULL;
}

void moveToHead(struct LinkedList *list, struct LinkedNode *node) {
    assert(list != NULL && node != NULL);
    errno = 0;

    if (node -> prev == NULL) {
        return;
    }

    // Remove node form its current position
    node -> prev -> next = node -> next;
    if (node -> next == NULL) {
        list -> tail = node -> prev;
    } else {
        node -> next -> prev = node -> prev;
    }

    // Insert node to head
    node -> next = list -> head;
    node -> prev = NULL;
    list -> head -> prev = node;
    list -> head = node;
}

void moveToTail(struct LinkedList *list, struct LinkedNode *node) {
    assert(list != NULL && node != NULL);
    errno = 0;

    if (node -> next == NULL) {
        return;
    }

     // Remove node form its current position
    node -> next -> prev = node -> prev;
    if (node -> prev == NULL) {
        list -> head = node -> next;
    } else {
        node -> prev -> next = node -> next;
    }

    // Insert node to tail
    node -> prev = list -> tail;
    node -> next = NULL;
    list -> tail -> next = node;
    list -> tail = node;
}

// Complexity: O(1)
void deleteNode(struct LinkedList *list, struct LinkedNode *node,
                void (*destroyDataFunc)(struct NodeData *)) {
    assert(list != NULL && node != NULL);
    errno = 0;
    if (list -> listSize == 0) {
        // Error: (Invalid Argument) List is empty
        errno = EINVAL;
        return;
    }

    // Maintain list
    if (node -> prev == NULL) {
        list -> head = node -> next;
    } else {
        node -> prev -> next = node -> next;
    }
    if (node -> next == NULL) {
        list -> tail = node -> prev;
    } else {
        node -> next -> prev = node -> prev;
    }

    list -> listSize--;

    // Free node
    if (destroyDataFunc != NULL) {
        destroyDataFunc(node -> data);
    } else {
        free(node -> data);
        node -> data = NULL;
    }
    free(node);
    node = NULL;
}
