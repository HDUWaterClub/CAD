#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <errno.h>

#include "datatypes.h"
#include "distance.h"

bool validateDataType(int dataType) {
    return dataType >= DATATYPE_RANGE_MIN && dataType <= DATATYPE_RANGE_MAX;
}

struct Vertex * makeVertex(int newx, int newy) {
    errno = 0;
    struct Vertex *newVtx = (struct Vertex *)malloc(sizeof(struct Vertex));
    if (newVtx == NULL) {
        free(newVtx);
        perror("makeVertex");
        return NULL;
    }
    newVtx -> x = newx;
    newVtx -> y = newy;
    return newVtx;
}

void destroyVertex(struct Vertex *vtx) {
    assert(vtx != NULL);
    free(vtx);
    vtx = NULL;
}

bool findVertexRule(const struct Vertex *cursorPt, const struct Vertex *vtx) {
    assert(cursorPt != NULL && vtx != NULL);
    return getManhattanDistance(cursorPt, vtx) <= FINDRULE_VARIATION;
}

struct Segment * makeSegment(struct Vertex *newLeftPt, struct Vertex *newRightPt) {
    assert(newLeftPt != NULL && newRightPt != NULL);
    errno = 0;
    struct Segment *newSeg = (struct Segment *)malloc(sizeof(struct Segment));
    if (newSeg == NULL) {
        free(newSeg);
        perror("makeSegment");
        return NULL;
    }

    if (newLeftPt -> x > newRightPt -> x) {
        struct Vertex *tmp = newRightPt;
        newRightPt = newLeftPt;
        newLeftPt = tmp;
    }

    newSeg -> leftPt = newLeftPt;
    newSeg -> rightPt = newRightPt;
    return newSeg;
}

void destroySegment(struct Segment *seg) {
    assert(seg != NULL);
    free(seg -> leftPt);
    seg -> leftPt = NULL;
    free(seg -> rightPt);
    seg -> rightPt = NULL;
    free(seg);
    seg = NULL;
}

bool findSegmentRule(const struct Vertex *cursorPt, const struct Segment *seg) {
    assert(cursorPt != NULL && seg != NULL);

    if (cursorPt -> x < seg -> leftPt -> x - FINDRULE_VARIATION ||
        cursorPt -> x > seg -> rightPt -> x + FINDRULE_VARIATION ||
        cursorPt -> y < min(seg -> leftPt -> y, seg -> rightPt-> y) - FINDRULE_VARIATION ||
        cursorPt -> y > max(seg -> leftPt -> y, seg -> rightPt-> y) + FINDRULE_VARIATION) {
            return false;
    }

    long long int leftVal = ((long long int)seg -> leftPt -> y - seg -> rightPt -> y) * cursorPt -> x
                        + ((long long int)seg -> rightPt -> x - seg -> leftPt -> x) * cursorPt -> y
                        + ((long long int)seg -> leftPt -> x * seg -> rightPt -> y - (long long int)seg -> rightPt -> x * seg -> leftPt -> y);
    leftVal *= leftVal;

    long long int rightVal = quickPow(FINDRULE_VARIATION, 2) * (quickPow(seg -> leftPt -> x - seg -> rightPt -> x, 2) + quickPow(seg -> leftPt -> y - seg -> rightPt -> y, 2));

    return leftVal <= rightVal;
}

struct Rectangle * makeRectangle(struct Vertex *newLowerLeftPt, struct Vertex *newUpperRightPt) {
    assert(newLowerLeftPt != NULL && newUpperRightPt != NULL);
    errno = 0;
    struct Rectangle *newRec = (struct Rectangle *)malloc(sizeof(struct Rectangle));
    if (newRec == NULL) {
        free(newRec);
        perror("makeRectangle");
        return NULL;
    }

    // Correct data
    int minx = min(newLowerLeftPt -> x, newUpperRightPt -> x);
    int miny = min(newLowerLeftPt -> y, newUpperRightPt -> y);
    int maxx = max(newLowerLeftPt -> x, newUpperRightPt -> x);
    int maxy = max(newLowerLeftPt -> y, newUpperRightPt -> y);
    newLowerLeftPt -> x = minx;
    newLowerLeftPt -> y = miny;
    newUpperRightPt -> x = maxx;
    newUpperRightPt -> y = maxy;

    newRec -> lowerLeftPt = newLowerLeftPt;
    newRec -> upperRightPt = newUpperRightPt;
    return newRec;
}

void destroyRectangle(struct Rectangle *rec) {
    assert(rec != NULL);
    free(rec -> lowerLeftPt);
    rec -> lowerLeftPt = NULL;
    free(rec -> upperRightPt);
    rec -> upperRightPt = NULL;
    free(rec);
    rec = NULL;
}

bool findRectangleRule(const struct Vertex *cursorPt, const struct Rectangle *rec) {
    assert(cursorPt != NULL && rec != NULL);
    return (
        cursorPt -> x >= rec -> lowerLeftPt -> x - FINDRULE_VARIATION &&
        cursorPt -> x <= rec -> upperRightPt-> x + FINDRULE_VARIATION &&
        cursorPt -> y >= rec -> lowerLeftPt -> y - FINDRULE_VARIATION &&
        cursorPt -> y <= rec -> upperRightPt-> y + FINDRULE_VARIATION
    );
}

struct Circle * makeCircle(struct Vertex *newCenterPt, int newRadius) {
    assert(newCenterPt != NULL);
    errno = 0;
    struct Circle *newCir = (struct Circle *)malloc(sizeof(struct Circle));
    if (newCir == NULL) {
        free(newCir);
        perror("makeCircle");
        return NULL;
    }
    newCir -> centerPt = newCenterPt;
    newCir -> radius = newRadius;
    return newCir;
}

void destoryCircle(struct Circle *cir) {
    assert(cir != NULL);
    free(cir -> centerPt);
    cir -> centerPt = NULL;
    free(cir);
    cir = NULL;
}

bool findCircleRule(const struct Vertex *cursorPt, const struct Circle *cir) {
    assert(cursorPt != NULL && cir != NULL);
    return getSqrEuclideanDistance(cir -> centerPt, cursorPt) <= quickPow(cir -> radius + FINDRULE_VARIATION, 2);
}

struct Ellipse * makeEllipse(struct Vertex *newCenterPt, int newMajorSemiAxis, int newMinorSemiAxis) {
    assert(newCenterPt != NULL);
    errno = 0;
    struct Ellipse *newElp = (struct Ellipse *)malloc(sizeof(struct Ellipse));
    if (newElp == NULL) {
        free(newElp);
        perror("makeEllipse");
        return NULL;
    }
    newElp -> centerPt = newCenterPt;
    newElp -> majorSemiAxis = newMajorSemiAxis;
    newElp -> minorSemiAxis = newMinorSemiAxis;
    return newElp;
}

void destroyEllipse(struct Ellipse *elp) {
    assert(elp != NULL);
    free(elp -> centerPt);
    elp -> centerPt = NULL;
    free(elp);
    elp = NULL;
}

bool findEllipseRule(const struct Vertex *cursorPt, const struct Ellipse *elp) {
    assert(cursorPt != NULL && elp != NULL);
    // Prevent possible overflow
    if (((long long int)elp -> minorSemiAxis + FINDRULE_VARIATION) * (cursorPt -> x - elp -> centerPt -> x) <= 1e9 &&
        ((long long int)elp -> majorSemiAxis + FINDRULE_VARIATION) * (cursorPt -> y - elp -> centerPt -> y) <= 1e9 &&
        ((long long int)elp -> majorSemiAxis + FINDRULE_VARIATION) * ((long long int)elp -> minorSemiAxis + FINDRULE_VARIATION) <= 1e9) {
            return quickPow(((long long int)elp -> minorSemiAxis + FINDRULE_VARIATION) * (cursorPt -> x - elp -> centerPt -> x), 2) + quickPow(((long long int)elp -> majorSemiAxis + FINDRULE_VARIATION) * (cursorPt -> y - elp -> centerPt -> y), 2) <= quickPow(((long long int)elp -> majorSemiAxis + FINDRULE_VARIATION) *
((long long int)elp -> minorSemiAxis + FINDRULE_VARIATION), 2);
    } else {
        return (long double)quickPow((cursorPt -> x - elp -> centerPt -> x), 2) / quickPow(elp -> majorSemiAxis, 2) + (long double)quickPow((cursorPt -> y - elp -> centerPt -> y), 2) / quickPow(elp -> minorSemiAxis, 2) <= 1.0;
    }
}

struct Text * makeText(struct Rectangle *newPosition, char *newContent, int newFontWidth, int newFontHeight) {
    assert(newPosition != NULL && newContent != NULL);
    errno = 0;
    struct Text *newText = (struct Text *)malloc(sizeof(struct Text));
    if (newText == NULL) {
        free(newText);
        perror("newText");
        return NULL;
    }
    newText -> position = newPosition;
    newText -> content = newContent;
    newText -> fontWidth = newFontWidth;
    newText -> fontHeight = newFontHeight;
    return newText;
}

void destoryText(struct Text *txt) {
    assert(txt != NULL);
    destroyRectangle(txt -> position);
    free(txt -> content);
    txt -> content = NULL;
    free(txt);
    txt = NULL;
}

bool findTextRule(const struct Vertex *cursorPt, const struct Text *txt) {
    assert(cursorPt != NULL && txt != NULL && txt -> position != NULL);
    return findRectangleRule(cursorPt, txt -> position);
}

void destroyRule(struct NodeData *data) {
    assert(data != NULL);
    switch(data -> type) {
        case DATATYPE_SEGMENT: {
            destroySegment((struct Segment *)data -> content);
            break;
        }
        case DATATYPE_RECTANGLE: {
            destroyRectangle((struct Rectangle *)data -> content);
            break;
        }
        case DATATYPE_CIRCLE: {
            destoryCircle((struct Circle *)data -> content);
            break;
        }
        case DATATYPE_ELLIPSE: {
            destroyEllipse((struct Ellipse *)data -> content);
            break;
        }
        case DATATYPE_TEXT: {
            destoryText((struct Text *)data -> content);
            break;
        }
    }
    free(data);
    data = NULL;
}

bool findRule(const void *vCursorPt, const struct LinkedNode *node) {
    assert(vCursorPt != NULL && node != NULL);
    if (node -> data == NULL) {
        return false;
    }

    struct Vertex *cursorPt = (struct Vertex *)vCursorPt;
    switch (node -> data -> type) {
        case DATATYPE_SEGMENT: {
            struct Segment *seg = (struct Segment *)node -> data -> content;
            return findSegmentRule(cursorPt, seg);
        }
        case DATATYPE_RECTANGLE: {
            struct Rectangle *rec = (struct Rectangle *)node -> data -> content;
            return findRectangleRule(cursorPt, rec);
        }
        case DATATYPE_CIRCLE: {
            struct Circle *cir = (struct Circle *)node -> data -> content;
            return findCircleRule(cursorPt, cir);
        }
        case DATATYPE_ELLIPSE: {
            struct Ellipse *elp = (struct Ellipse *)node -> data -> content;
            return findEllipseRule(cursorPt, elp);
        }
        case DATATYPE_TEXT: {
            struct Text *txt = (struct Text *)node -> data -> content;
            return findTextRule(cursorPt, txt);
        }
        default: {
            return false;
        }
    }
}
