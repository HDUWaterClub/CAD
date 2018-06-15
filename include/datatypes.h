#ifndef DATATYPES_H_
#define DATATYPES_H_

#include "misc.h"
#include "linkedlist.h"

#define DATATYPE_UNDEFINED 0
#define DATATYPE_SEGMENT 1
#define DATATYPE_RECTANGLE 2
#define DATATYPE_CIRCLE 3
#define DATATYPE_ELLIPSE 4
#define DATATYPE_TEXT 5

#define DATATYPE_TEXT_MAX_LENGTH 255

#define DATATYPE_RANGE_MIN 1
#define DATATYPE_RANGE_MAX 5

#define FINDRULE_VARIATION 10

#define CURSOR_OFF_ELEMENT 0
#define CURSOR_ON_ELEMENT 1

#define CURSOR_SEGMENT_LEFTPT 2
#define CURSOR_SEGMENT_RIGHTPT 3

#define CURSOR_RECTANGLE_LOWERLEFT 4
#define CURSOR_RECTANGLE_UPPERLEFT 5
#define CURSOR_RECTANGLE_LOWERRIGHT 6
#define CURSOR_RECTANGLE_UPPERRIGHT 7

#define CURSOR_CIRCLE_EDGE 8
#define CURSOR_ELLIPSE_EDGE 9

bool validateDataType(int dataType);

struct Vertex {
    int x, y;
};
struct Vertex * makeVertex(int newx, int newy);
bool findVertexRule(const struct Vertex *cursorPt, const struct Vertex *vtx);
void destroyVertex(struct Vertex *vtx);

struct Segment {
    struct Vertex *leftPt, *rightPt;
};
struct Segment * makeSegment(struct Vertex *newLeftPt, struct Vertex *newRightPt);
bool findSegmentRule(const struct Vertex *cursorPt, const struct Segment *seg);
void destroySegment(struct Segment *seg);

struct Rectangle {
    struct Vertex *lowerLeftPt, *upperRightPt;
};
struct Rectangle * makeRectangle(struct Vertex *newLowerLeftPt, struct Vertex *newUpperRightPt);
bool findRectangleRule(const struct Vertex *cursorPt, const struct Rectangle *rec);
void destroyRectangle(struct Rectangle *rec);

struct Circle {
    struct Vertex *centerPt;
    int radius;
};
struct Circle * makeCircle(struct Vertex *newCenterPt, int newRadius);
bool findCircleRule(const struct Vertex *cursorPt, const struct Circle *cir);
void destoryCircle(struct Circle *cir);

struct Ellipse {
    struct Vertex *centerPt;
    int majorSemiAxis, minorSemiAxis;
};
struct Ellipse * makeEllipse(struct Vertex *newCenterPt, int newMajorSemiAxis, int newMinorSemiAxis);
bool findEllipseRule(const struct Vertex *cursorPt, const struct Ellipse *elp);
void destroyEllipse(struct Ellipse *elp);

struct Text {
    struct Rectangle *position;
    int fontWidth, fontHeight;
    char *content;
};
struct Text * makeText(struct Rectangle *newPosition, char *newContent, int newFontWidth, int newFontHeight);
bool findTextRule(const struct Vertex *cursorPt, const struct Text *txt);
void destroyText(struct Text *txt);

void destroyRule(struct NodeData *data);
bool findRule(const void *vCursorPt, const struct LinkedNode *node);

#endif
