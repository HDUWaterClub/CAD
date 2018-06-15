#include <math.h>
#include <assert.h>

#include "distance.h"
#include "draw.h"
#include "layout.h"

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

void drawNodeData(struct NodeData *nodeData, color_t fgColor) {
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
            break;
        }
        default: {
            break;
        }
    }
    setcolor(prevFgColor);
}

void redrawAll(struct LinkedList *list, color_t fgColor) {
    clearCanvas();
    struct LinkedNode *cntNode = list -> head;
    while (cntNode != NULL && cntNode -> data != NULL) {
        drawNodeData(cntNode -> data, fgColor);
        cntNode = cntNode -> next;
    }
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
            redrawAll(list, drawnFgColor);
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
    int xType = assistId / 3, yType = assistId % 3;

    getStartEndPts(data, startPt, endPt);
    assert(startPt != NULL && endPt != NULL && *startPt != NULL && *endPt != NULL);

    struct Vertex *cntEndPt = makeVertex((*endPt) -> x, (*endPt) -> y);
    drawShape(*startPt, cntEndPt, data -> type, SHAPE_DEFAULT_COLOR);

    while (true) {
        mouse_msg m = getmouse();

        // Clear previous
        redrawAll(list, SHAPE_DEFAULT_COLOR);
        drawNodeData(data, EDIT_ASSIST_COLOR);

        cntEndPt -> x = m.x;
        cntEndPt -> y = m.y;
        getRealPosition(cntEndPt);

        if (xType == 2) {
            cntEndPt -> y = (*endPt) -> y;
        }
        if (yType == 2) {
            cntEndPt -> x = (*endPt) -> x;
        }

        drawShape(*startPt, cntEndPt, data -> type, SHAPE_DEFAULT_COLOR);

        if (m.is_up() && m.is_left()) {
            destroyVertex(*endPt);
            *endPt = cntEndPt;
            return;
        }
    }
}

void trackShape(struct LinkedList *list, struct NodeData *data, struct Vertex *cursorPt,
                struct Vertex **startPt, struct Vertex **endPt) {
    assert(list != NULL && data != NULL && cursorPt != NULL);

    struct Vertex *cntStartPt = NULL, *cntEndPt = NULL;
    getStartEndPts(data, &cntStartPt, &cntEndPt);

    while (true) {
        mouse_msg m = getmouse();

        if (whichArea(m.x) == AREA_MENU) {
            destroyVertex(cntStartPt);
            destroyVertex(cntEndPt);
            break;
        }

        int prevx = cursorPt -> x, prevy = cursorPt -> y;
        cursorPt -> x = m.x;
        cursorPt -> y = m.y;
        getRealPosition(cursorPt);
        int deltax = cursorPt -> x - prevx, deltay = cursorPt -> y - prevy;

        cntStartPt -> x += deltax;
        cntStartPt-> y += deltay;
        cntEndPt -> x += deltax;
        cntEndPt-> y += deltay;

        redrawAll(list, SHAPE_DEFAULT_COLOR);
        drawNodeData(data, EDIT_ASSIST_COLOR);
        drawShape(cntStartPt, cntEndPt, data -> type, SHAPE_DEFAULT_COLOR);

        if (m.is_up()) {
            *startPt = cntStartPt;
            *endPt = cntEndPt;
            return;
        }
    }

    startPt = NULL;
    endPt = NULL;
}
