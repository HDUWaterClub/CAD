#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

void moveShape(struct LinkedList *list, struct LinkedNode *node, int mx, int my) {
    struct Vertex *cursorPt = makeVertex(mx, my);
    getRealPosition(cursorPt);
    struct Vertex *startPt = NULL, *endPt = NULL;
    trackShape(list, node -> data, cursorPt, &startPt, &endPt);
    destroyVertex(cursorPt);

    if (startPt != NULL && endPt != NULL) {
        if (node -> data -> type == DATATYPE_TEXT) {
            editText(node, startPt, endPt);
        } else {
            editShape(node, startPt, endPt);
        }
    }

    redrawAll(list, SHAPE_DEFAULT_COLOR, false);
}

int editMode(struct LinkedList *list, struct LinkedNode *node) {
    assert(node != NULL && node -> data != NULL);

    changeButtonText(BUTTON_TYPE_CLEAR, (char *)"DELETE");
    drawButton(BUTTON_TYPE_CLEAR, BUTTON_STATE_AVAILABLE);

    bool shouldReturn = false, isFirst = true;
    int returnVal = BUTTON_NON_ACTIVE;

    while (true) {
        drawNodeData(node -> data, EDIT_ASSIST_COLOR, false);
        drawEditAssist(node -> data);

        while (true) {
            mouse_msg m = getmouse();

            if (m.is_up()) {
                isFirst = false;
                continue;
            }

            int cntArea = whichArea(m.x);
            if (isFirst && m.is_move()) {
                moveShape(list, node, m.x, m.y);
                isFirst = false;
                break;
            } else if (cntArea == AREA_MENU && m.is_down()) {
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
                if (m.is_down()) {
                    struct Vertex *cursorPt = makeVertex(m.x, m.y);
                    getRealPosition(cursorPt);
                    bool isInShape = (findRule(cursorPt, node) || isInAssistArea(cursorPt));
                    destroyVertex(cursorPt);

                    if (isInShape) {
                        if (m.is_left()) {
                            struct Vertex *cursorPt = makeVertex(m.x, m.y);
                            getRealPosition(cursorPt);
                            int assistId = getAssistId(cursorPt);
                            destroyVertex(cursorPt);

                            if (assistId == -1) {
                                moveShape(list, node, m.x, m.y);
                                break;
                            } else {
                                struct Vertex *startPt = NULL, *endPt = NULL;
                                trackEditPts(list, node -> data, assistId, &startPt, &endPt);
                                assert(startPt != NULL && endPt != NULL);

                                if (node -> data -> type == DATATYPE_TEXT) {
                                    struct Text *txt = (struct Text *)node -> data -> content;
                                    LOGFONT cntFont = defaultFont;
                                    cntFont.lfHeight = abs(startPt -> y - endPt -> y);
                                    setfont(&cntFont);

                                    startPt -> x = min(startPt -> x, endPt -> x);
                                    startPt -> y = min(startPt -> y, endPt -> y);
                                    endPt -> x = startPt -> x + textwidth(txt -> content);
                                    endPt -> y = startPt -> y + cntFont.lfHeight;

                                    editText(node, startPt, endPt);
                                } else {
                                    editShape(node, startPt, endPt);
                                }

                                redrawAll(list, SHAPE_DEFAULT_COLOR, false);
                                break;
                            }
                        } else if (m.is_right()) {
                            // Move current shape to list head
                            moveToHead(list, node);
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
        }

        redrawAll(list, SHAPE_DEFAULT_COLOR, false);
        if (shouldReturn) {
            break;
        }
    }

    changeButtonText(BUTTON_TYPE_CLEAR, (char *)"CLEAR");
    drawButton(BUTTON_TYPE_CLEAR, BUTTON_STATE_AVAILABLE);
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

            //struct LinkedNode * newNode = saveShape(list, startPt, endPt, shapeType);
            saveShape(list, startPt, endPt, shapeType);
            redrawAll(list, SHAPE_DEFAULT_COLOR, false);
            break;
            //return editMode(list, newNode);
        }
    }
    return cntButtonId;
}

int textMode(struct LinkedList *list) {
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
            struct Vertex *startPt = makeVertex(m.x, m.y);
            getRealPosition(startPt);

            char *content = (char *)malloc(sizeof(char) * DATATYPE_TEXT_MAX_LENGTH);
            setViewPort(AREA_ALL);
            while (inputbox_getline((char *)"Add Text", (char *)"Please leave something here and press ENTER.\nLeave it blank to exit...", content, DATATYPE_TEXT_MAX_LENGTH) == 0) {
                if (strlen(content) == 0) {
                    free(content);
                    content = NULL;
                    break;
                }
            }
            setViewPort(AREA_CANVAS);

            if (content != NULL) {
                struct Vertex *endPt = getTextEndPt(startPt, content, &defaultFont);
                drawText(startPt, endPt, content, &defaultFont, SHAPE_DEFAULT_COLOR, BLACK);
                saveText(list, startPt, endPt, content, defaultFont.lfWidth, defaultFont.lfHeight);
            }

            redrawAll(list, SHAPE_DEFAULT_COLOR, false);
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
                nextButtonId = textMode(&list);
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
            drawButton(cntButtonId, BUTTON_STATE_AVAILABLE);
        }
        if (nextButtonId >= 0 && nextButtonId < BUTTON_NUMBER) {
            drawButton(nextButtonId, BUTTON_STATE_AVAILABLE + BUTTON_STATE_ACTIVE);
        }
        cntButtonId = nextButtonId;
    }

    cleardevice();
    destroyLinkedList(&list, destroyRule);
    closegraph();
    return 0;
}
