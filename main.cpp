#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>

#include "graphics.h"

#include "misc.h"
#include "distance.h"
#include "linkedlist.h"
#include "datatypes.h"

#include "draw.h"
#include "layout.h"

int cntButtonId;

int editMode(struct LinkedList *list, struct LinkedNode *node) {
    assert(node != NULL);

    changeButtonText(BUTTON_TYPE_CLEAR, "DELETE");
    drawButton(BUTTON_TYPE_CLEAR, BUTTON_STATE_INACTIVE);

    bool shouldReturn = false;
    int returnVal = BUTTON_NON_ACTIVE;

    while (true) {
        drawNodeData(node -> data, EDIT_ASSIST_COLOR);
        drawEditAssist(node -> data);

        while (true) {
            mouse_msg m = getmouse();

            if (!m.is_down()) {
                continue;
            }

            int cntArea = whichArea(m.x);
            if (cntArea == AREA_MENU) {
                // When user clicked left button in menu area,
                // decide about changing work mode
                int nextButtonId = getButtonId(m.x, m.y);
                if (nextButtonId == BUTTON_TYPE_CLEAR) {
                    deleteNode(list, node, destroyRule);
                    returnVal = BUTTON_NON_ACTIVE;
                    shouldReturn = true;
                    break;
                } else if (nextButtonId != BUTTON_NON_CHANGED) {
                    returnVal = nextButtonId;
                    shouldReturn = true;
                    break;
                }
            } else if (cntArea == AREA_CANVAS) {
                if (m.is_move()) {
                    while (true) {

                    }
                }

                struct Vertex *cursorPt = makeVertex(m.x, m.y);
                getRealPosition(cursorPt);
                bool isInShape = findRule(cursorPt, node);
                destroyVertex(cursorPt);

                if (isInShape) {
                    if (m.is_left()) {

                    } else if (m.is_right()) {
                        // Move current shape to list head
                        moveToHead(list, node);
                        returnVal = BUTTON_NON_ACTIVE;
                        shouldReturn = true;
                        break;
                    }
                } else {
                    if (m.is_left()) {
                        struct Vertex *cursorPt = makeVertex(m.x, m.y);
                        getRealPosition(cursorPt);
                        struct LinkedNode *nextNode = findNode(list, cursorPt, findRule);
                        destroyVertex(cursorPt);

                        if (nextNode == NULL) {
                            returnVal = BUTTON_NON_ACTIVE;
                            shouldReturn = true;
                            break;
                        } else {
                            node = nextNode;
                            break;
                        }
                    }
                }
            }
        }

        redrawAll(list, SHAPE_DEFAULT_COLOR);

        if (shouldReturn) {
            break;
        }
    }

    changeButtonText(BUTTON_TYPE_CLEAR, "CLEAR");
    drawButton(BUTTON_TYPE_CLEAR, BUTTON_STATE_INACTIVE);
    return returnVal;
}

int selectionMode(struct LinkedList *list) {
    assert(list != NULL);
    while (true) {
        mouse_msg m = getmouse();
        if (m.is_left() && m.is_down()) {
            int cntArea = whichArea(m.x);
            if (cntArea == AREA_MENU) {
                // When user clicked left button in menu area,
                // decide about changing work mode
                int nextButtonId = getButtonId(m.x, m.y);
                if (nextButtonId != BUTTON_NON_CHANGED) {
                    return nextButtonId;
                }
            } else if (cntArea == AREA_CANVAS) {
                // When user clicked right button in canvas area,
                // select element and edit it
                struct Vertex *cursorPt = makeVertex(m.x, m.y);
                getRealPosition(cursorPt);
                struct LinkedNode *node = findNode(list, cursorPt, findRule);
                destroyVertex(cursorPt);

                if (node != NULL) {
                    // Edit selected element
                    return editMode(list, node);
                }
            }
        }
    }
    return BUTTON_NON_ACTIVE;
}

int drawMode(struct LinkedList *list, int shapeType) {
    assert(list != NULL);
    while (true) {
        mouse_msg m = getmouse();
        // Only left key clicking down matters
        if (!m.is_left() || !m.is_down()) {
            continue;
        }
        int cntArea = whichArea(m.x);
        if (cntArea == AREA_MENU) {
            // When user clicked left button in menu area,
            // decide about changing work mode
            int nextButtonId = getButtonId(m.x, m.y);
            if (nextButtonId != BUTTON_NON_CHANGED) {
                return cntButtonId == nextButtonId ? BUTTON_NON_ACTIVE : nextButtonId;
            }
        } else if (cntArea == AREA_CANVAS) {
            // When user clicked left button in canvas area,
            // start to trace cursor
            struct Vertex *startPt = makeVertex(m.x, m.y);
            getRealPosition(startPt);
            struct Vertex *endPt = trackEndPt(list, startPt, shapeType, SHAPE_DEFAULT_COLOR, SHAPE_DEFAULT_COLOR);

            struct LinkedNode * newNode = saveShape(list, startPt, endPt, shapeType);
            if (newNode == NULL) {
                destroyVertex(startPt);
                destroyVertex(endPt);
            }
            redrawAll(list, SHAPE_DEFAULT_COLOR);
            // return editMode(list, newNode);
            break;
        }
    }
    return cntButtonId;
}

int main()
{
    init();

    struct LinkedList list;
    initLinkedList(&list);

    cntButtonId = BUTTON_NON_ACTIVE;

    while (cntButtonId != BUTTON_TYPE_EXIT && cntButtonId >= 0) {
        int nextButtonId = cntButtonId;

        switch (cntButtonId) {
            case BUTTON_NON_ACTIVE: {
                nextButtonId = selectionMode(&list);
                break;
            }
            case BUTTON_TYPE_SEGMENT: {
                nextButtonId = drawMode(&list, DATATYPE_SEGMENT);
                break;
            }
            case BUTTON_TYPE_RECTANGLE: {
                nextButtonId = drawMode(&list, DATATYPE_RECTANGLE);
                break;
            }
            case BUTTON_TYPE_CIRCLE: {
                nextButtonId = drawMode(&list, DATATYPE_CIRCLE);
                break;
            }
            case BUTTON_TYPE_ELLIPSE: {
                nextButtonId = drawMode(&list, DATATYPE_ELLIPSE);
                break;
            }
            case BUTTON_TYPE_TEXT: {
                nextButtonId = drawMode(&list, DATATYPE_TEXT);
                break;
            }
            case BUTTON_TYPE_CLEAR: {
                destroyLinkedList(&list, destroyRule);
                initLinkedList(&list);
                clearCanvas();
                nextButtonId = BUTTON_NON_ACTIVE;
                break;
            }
            default: {
                nextButtonId = -1;
                break;
            }
        }

        if (cntButtonId >= 0 && cntButtonId < BUTTON_NUMBER) {
            drawButton(cntButtonId, BUTTON_STATE_INACTIVE);
        }
        if (nextButtonId >= 0 && nextButtonId < BUTTON_NUMBER) {
            drawButton(nextButtonId, BUTTON_STATE_ACTIVE);
        }
        cntButtonId = nextButtonId;
    }

    cleardevice();
    destroyLinkedList(&list, destroyRule);
    closegraph();
    return 0;
}
