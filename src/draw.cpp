#include <stdio.h>
#include <math.h>
#include <assert.h>

#include "distance.h"
#include "draw.h"
#include "layout.h"

LOGFONT defaultFont;

void fillBlock(int minx, int maxy, int maxx, int miny, int fillColor) {
    color_t prevFillColor = getfillcolor();
    setfillcolor(fillColor);
    bar(minx, maxy, maxx, miny);
    setfillcolor(prevFillColor);
}

void getRealPosition(struct Vertex *cntPt) {
    assert(cntPt != NULL);
    int cntCanvasMinx = -1, cntCanvasMiny = -1, cntCanvasMaxx = -1, cntCanvasMaxy = -1;
    getviewport(&cntCanvasMinx, &cntCanvasMiny, &cntCanvasMaxx, &cntCanvasMaxy);
    assert(cntCanvasMinx >= 0 && cntCanvasMiny >= 0 && cntCanvasMaxx >= 0 && cntCanvasMaxy >= 0);

    cntPt -> x -= cntCanvasMinx;
    cntPt -> y -= cntCanvasMiny;
}

void drawShape(struct Vertex *startPt, struct Vertex *endPt, int shapeType, color_t fgColor) {
    assert(startPt != NULL && endPt != NULL);
    color_t prevFgColor = getcolor();
    setcolor(fgColor);

    switch (shapeType) {
        case DATATYPE_SEGMENT: {
            line(startPt -> x, startPt -> y, endPt -> x, endPt -> y);
            break;
        }
        case DATATYPE_RECTANGLE: {
            rectangle(min(startPt -> x, endPt -> x), max(startPt -> y, endPt -> y),
                    max(startPt -> x, endPt -> x), min(startPt -> y, endPt -> y));
            break;
        }
        case DATATYPE_CIRCLE: {
            struct Vertex *centerPt = makeVertex((startPt -> x + endPt -> x) / 2, (startPt -> y + endPt -> y) / 2);
            int radius = sqrt(getSqrEuclideanDistance(centerPt, endPt));
            circle(centerPt -> x, centerPt -> y, radius);
            destroyVertex(centerPt);
            break;
        }
        case DATATYPE_ELLIPSE: {
            struct Vertex *centerPt = makeVertex((startPt -> x + endPt -> x) / 2, (startPt -> y + endPt -> y) / 2);
            int majorSemiAxis = abs(endPt -> x - centerPt -> x);
            int minorSemiAxis = abs(endPt -> y - centerPt -> y);
            ellipse(centerPt -> x, centerPt -> y, 0, 360, majorSemiAxis, minorSemiAxis);
            destroyVertex(centerPt);
            break;
        }
        default: {
            break;
        }
    }
    setcolor(prevFgColor);
}

struct LinkedNode *saveShape(struct LinkedList *list, struct Vertex *startPt, struct Vertex *endPt, int shapeType) {
    assert(list != NULL && startPt != NULL && endPt != NULL);


    switch (shapeType) {
        case DATATYPE_SEGMENT: {
            if (getManhattanDistance(startPt, endPt) <= FINDRULE_VARIATION) {
                break;
            }
            struct LinkedNode *res = addNode(list, makeData(makeSegment(startPt, endPt), DATATYPE_SEGMENT));
            return res;
        }
        case DATATYPE_RECTANGLE: {
            if (abs(startPt -> x - endPt -> x) <= FINDRULE_VARIATION || abs(startPt -> y - endPt -> y) <= FINDRULE_VARIATION) {
                break;
            }
            struct LinkedNode *res = addNode(list, makeData(makeRectangle(startPt, endPt), DATATYPE_RECTANGLE));
            return res;
        }
        case DATATYPE_CIRCLE: {
            struct Vertex *centerPt = makeVertex((startPt -> x + endPt -> x) / 2, (startPt -> y + endPt -> y) / 2);
            int radius = sqrt(getSqrEuclideanDistance(centerPt, endPt));
            if (radius <= FINDRULE_VARIATION) {
                break;
            }
            struct LinkedNode *res = addNode(list, makeData(makeCircle(centerPt, radius), DATATYPE_CIRCLE));
            destroyVertex(startPt);
            destroyVertex(endPt);
            return res;
        }
        case DATATYPE_ELLIPSE: {
            struct Vertex *centerPt = makeVertex((startPt -> x + endPt -> x) / 2, (startPt -> y + endPt -> y) / 2);
            int majorSemiAxis = abs(endPt -> x - centerPt -> x);
            int minorSemiAxis = abs(endPt -> y - centerPt -> y);
            if (majorSemiAxis <= FINDRULE_VARIATION || minorSemiAxis <= FINDRULE_VARIATION) {
                break;
            }
            struct LinkedNode *res = addNode(list, makeData(makeEllipse(centerPt, majorSemiAxis, minorSemiAxis), DATATYPE_ELLIPSE));
            destroyVertex(startPt);
            destroyVertex(endPt);
            return res;
        }
        case DATATYPE_TEXT: {
            break;
        }
    }

    destroyVertex(startPt);
    destroyVertex(endPt);
    return NULL;
}

struct LinkedNode *saveText(struct LinkedList *list, struct Vertex *startPt, struct Vertex *endPt, char *text, int fontWidth, int fontHeight) {
    struct LinkedNode *res = addNode(list, makeData(makeText(makeRectangle(startPt, endPt), text, fontWidth, fontHeight), DATATYPE_TEXT));
    return res;
}

void editShape(struct LinkedNode *node, struct Vertex *startPt, struct Vertex *endPt) {
    assert(node != NULL && node -> data != NULL && startPt != NULL && endPt != NULL);

    switch (node -> data -> type) {
        case DATATYPE_SEGMENT: {
            if (getManhattanDistance(startPt, endPt) <= FINDRULE_VARIATION) {
                break;
            }
            editNode(node, makeData(makeSegment(startPt, endPt), DATATYPE_SEGMENT), destroyRule);
            return;
        }
        case DATATYPE_RECTANGLE: {
            if (abs(startPt -> x - endPt -> x) <= FINDRULE_VARIATION || abs(startPt -> y - endPt -> y) <= FINDRULE_VARIATION) {
                break;
            }
            editNode(node, makeData(makeRectangle(startPt, endPt), DATATYPE_RECTANGLE), destroyRule);
            return;
        }
        case DATATYPE_CIRCLE: {
            struct Vertex *centerPt = makeVertex((startPt -> x + endPt -> x) / 2, (startPt -> y + endPt -> y) / 2);
            int radius = sqrt(getSqrEuclideanDistance(centerPt, endPt));
            if (radius <= FINDRULE_VARIATION) {
                break;
            }
            editNode(node, makeData(makeCircle(centerPt, radius), DATATYPE_CIRCLE), destroyRule);
            destroyVertex(startPt);
            destroyVertex(endPt);
            return;
        }
        case DATATYPE_ELLIPSE: {
            struct Vertex *centerPt = makeVertex((startPt -> x + endPt -> x) / 2, (startPt -> y + endPt -> y) / 2);
            int majorSemiAxis = abs(endPt -> x - centerPt -> x);
            int minorSemiAxis = abs(endPt -> y - centerPt -> y);
            if (majorSemiAxis <= FINDRULE_VARIATION || minorSemiAxis <= FINDRULE_VARIATION) {
                break;
            }
            editNode(node, makeData(makeEllipse(centerPt, majorSemiAxis, minorSemiAxis), DATATYPE_ELLIPSE), destroyRule);
            destroyVertex(startPt);
            destroyVertex(endPt);
            return;
        }
        case DATATYPE_TEXT: {
            break;
        }
    }
    destroyVertex(startPt);
    destroyVertex(endPt);
}

void editText(struct LinkedNode *node, struct Vertex *startPt, struct Vertex *endPt,
              char *text, int fontWidth, int fontHeight) {
    assert(node != NULL && node -> data != NULL && startPt != NULL && endPt != NULL);

    if (text == NULL || fontWidth < 0 || fontHeight < 0) {
        struct Text *txt = (struct Text *)node -> data -> content;
        text = (char *)malloc(sizeof(char) * DATATYPE_TEXT_MAX_LENGTH);
        strncpy(text, txt -> content, DATATYPE_TEXT_MAX_LENGTH);
        fontWidth = txt -> fontWidth;
        fontHeight = txt -> fontHeight;
    }

    if (abs(startPt -> x - endPt -> x) <= FINDRULE_VARIATION || abs(startPt -> y - endPt -> y) <= FINDRULE_VARIATION) {
        return;
    }

    editNode(node, makeData(makeText(makeRectangle(startPt, endPt), text, fontWidth, fontHeight), DATATYPE_TEXT), destroyRule);
}

void drawNodeData(struct NodeData *nodeData, color_t fgColor, bool isDraft) {
    assert(nodeData != NULL);
    color_t prevFgColor = getcolor();
    setcolor(fgColor);

    switch (nodeData -> type) {
        case DATATYPE_SEGMENT: {
            struct Segment *seg = (struct Segment *)nodeData -> content;
            line(seg -> leftPt -> x, seg -> leftPt -> y, seg -> rightPt -> x, seg -> rightPt -> y);
            break;
        }
        case DATATYPE_RECTANGLE: {
            struct Rectangle *rec = (struct Rectangle *)nodeData -> content;
            rectangle(rec -> lowerLeftPt -> x, rec -> upperRightPt -> y, rec -> upperRightPt -> x, rec -> lowerLeftPt -> y);
            break;
        }
        case DATATYPE_CIRCLE: {
            struct Circle *cir = (struct Circle *)nodeData -> content;
            circle(cir -> centerPt -> x, cir -> centerPt -> y, cir -> radius);
            break;
        }
        case DATATYPE_ELLIPSE: {
            struct Ellipse * elp = (struct Ellipse *)nodeData -> content;
            ellipse(elp -> centerPt -> x, elp -> centerPt -> y, 0, 360, elp -> majorSemiAxis, elp -> minorSemiAxis);
            break;
        }
        case DATATYPE_TEXT: {
            struct Text *txt = (struct Text *)nodeData -> content;
            struct Rectangle *pos = (struct Rectangle *)txt -> position;

            if (isDraft) {
                LOGFONT cntFont = defaultFont;
                cntFont.lfQuality = NONANTIALIASED_QUALITY;
                drawText(pos -> lowerLeftPt, pos -> upperRightPt, txt -> content, &cntFont, fgColor, BLACK);
            } else {
                drawText(pos -> lowerLeftPt, pos -> upperRightPt, txt -> content, &defaultFont, fgColor, BLACK);
            }

            break;
        }
        default: {
            break;
        }
    }
    setcolor(prevFgColor);
}

void redrawAll(struct LinkedList *list, color_t fgColor, bool isDraft) {
    clearCanvas();
    struct LinkedNode *cntNode = list -> head;
    while (cntNode != NULL && cntNode -> data != NULL) {
        drawNodeData(cntNode -> data, fgColor, isDraft);
        cntNode = cntNode -> next;
    }
}

struct Vertex * getTextEndPt(struct Vertex *startPt, char *text, LOGFONT *font) {
    LOGFONT cntFont = *font;
    cntFont.lfWidth = 0;
    setfont(&cntFont);

    int cntTextHeight = textheight(text);
    int cntTextWidth = textwidth(text);
    setfont(&defaultFont);

    return makeVertex(startPt -> x + cntTextWidth, startPt -> y + cntTextHeight);
}

void drawText(struct Vertex *startPt, struct Vertex *endPt, char *text,
              LOGFONT *font, color_t textColor, color_t fillColor) {
    assert(startPt != NULL && text != NULL && text != NULL && font != NULL);

    color_t prevColor = getcolor(), prevFillColor = getfillcolor();

    setcolor(textColor);
    setfontbkcolor(fillColor);

    LOGFONT cntFont = *font;
    cntFont.lfWidth = 0;
    cntFont.lfHeight = abs(endPt -> y - startPt -> y);

    setfont(&cntFont);
    int minx = min(startPt -> x, endPt -> x), miny = min(startPt -> y, endPt -> y);
    //int maxx = max(startPt -> x, endPt -> x), maxy = max(startPt -> y, endPt -> y);
    int maxx = minx + textwidth(text), maxy = miny + textheight(text);
    outtextrect(minx, miny, maxx, maxy, text);

    setcolor(prevColor);
    setfillcolor(prevFillColor);
    setfont(&defaultFont);
}

struct Vertex * trackEndPt(struct LinkedList *list, struct Vertex *startPt,
                        int shapeType, color_t newFgColor, color_t drawnFgColor) {
    assert(list != NULL && startPt != NULL);

    struct Vertex *cntEndPt = makeVertex(startPt -> x, startPt -> y);
    drawShape(startPt, cntEndPt, shapeType, newFgColor);
    while (true) {
        mouse_msg m = getmouse();

        if (m.is_move()) {
            // Clear previous segment
            redrawAll(list, drawnFgColor, true);
            // Draw current segment
            cntEndPt -> x = m.x;
            cntEndPt -> y = m.y;
            getRealPosition(cntEndPt);
            drawShape(startPt, cntEndPt, shapeType, newFgColor);
        }

        if (m.is_left() && m.is_up()) {
            // End tracking when left button is up
            return cntEndPt;
        }
    }
}

void trackEditPts(struct LinkedList *list, struct NodeData *data, int assistId,
                struct Vertex **startPt, struct Vertex **endPt) {
    assert(list != NULL && data!= NULL && *startPt == NULL && *endPt == NULL);

    const int xType = assistId / 3, yType = assistId % 3;

    getStartEndPts(data, startPt, endPt, assistId);
    assert(startPt != NULL && endPt != NULL && *startPt != NULL && *endPt != NULL);

    const int origx = (*endPt) -> x, origy = (*endPt) -> y;

    while (true) {
        mouse_msg m = getmouse();

        (*endPt) -> x = m.x;
        (*endPt) -> y = m.y;
        getRealPosition(*endPt);

        if (xType == 2) {
            (*endPt) -> x = origx;
        }
        if (yType == 2) {
            (*endPt) -> y = origy;
        }

        redrawAll(list, SHAPE_DEFAULT_COLOR, true);
        drawNodeData(data, EDIT_ASSIST_COLOR, true);

        if (data -> type == DATATYPE_TEXT) {
            LOGFONT cntFont = defaultFont;
            cntFont.lfHeight += abs((*endPt) -> x - origx);
            cntFont.lfQuality = NONANTIALIASED_QUALITY;

            struct Text *txt = (struct Text *)data -> content;
            drawText(*startPt, *endPt,  txt -> content, &cntFont, SHAPE_DEFAULT_COLOR, CANVAS_COLOR);
        } else {
            drawShape(*startPt, *endPt, data -> type, SHAPE_DEFAULT_COLOR);
        }

        if (m.is_up()) {
            return;
        }
    }

    destroyVertex(*startPt);
    destroyVertex(*endPt);
}

void trackShape(struct LinkedList *list, struct NodeData *data, struct Vertex *cursorPt,
                struct Vertex **startPt, struct Vertex **endPt) {
    assert(list != NULL && data != NULL && cursorPt != NULL && *startPt == NULL && *endPt == NULL);

    getStartEndPts(data, startPt, endPt);
    assert(startPt != NULL && endPt != NULL && *startPt != NULL && *endPt != NULL);

    while (true) {
        mouse_msg m = getmouse();

        if (whichArea(m.x) == AREA_MENU) {
            break;
        }

        int prevx = cursorPt -> x, prevy = cursorPt -> y;
        cursorPt -> x = m.x;
        cursorPt -> y = m.y;
        getRealPosition(cursorPt);
        int deltax = cursorPt -> x - prevx, deltay = cursorPt -> y - prevy;

        (*startPt) -> x += deltax;
        (*startPt) -> y += deltay;
        (*endPt) -> x += deltax;
        (*endPt) -> y += deltay;

        redrawAll(list, SHAPE_DEFAULT_COLOR, true);
        drawNodeData(data, EDIT_ASSIST_COLOR, true);

        if (data -> type == DATATYPE_TEXT) {
            struct Text *txt = (struct Text *)data -> content;
            LOGFONT cntFont = defaultFont;
            cntFont.lfQuality = NONANTIALIASED_QUALITY;
            drawText(*startPt, *endPt, txt -> content, &cntFont, SHAPE_DEFAULT_COLOR, CANVAS_COLOR);
        } else {
            drawShape(*startPt, *endPt, data -> type, SHAPE_DEFAULT_COLOR);
        }

        if (m.is_up()) {
            return;
        }
    }

    destroyVertex(*startPt);
    destroyVertex(*endPt);
}
