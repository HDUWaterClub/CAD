#ifndef DRAW_H_
#define DRAW_H_

#include "graphics.h"

#include "linkedlist.h"
#include "datatypes.h"

#define SHAPE_DEFAULT_COLOR WHITE

extern LOGFONT defaultFont;

void fillBlock(int minx, int maxy, int maxx, int miny, int fillColor);

void getRealPosition(struct Vertex *cntPt);

// Draw functions
void drawShape(struct Vertex *startPt, struct Vertex *endPt, int shapeType, color_t fgColor);
struct LinkedNode *saveShape(struct LinkedList *list, struct Vertex *startPt, struct Vertex *endPt, int shapeType);
struct LinkedNode *saveText(struct LinkedList *list, struct Vertex *startPt, struct Vertex *endPt, char *text, int fontWidth, int fontHeight);
void editShape(struct LinkedNode *node, struct Vertex *startPt, struct Vertex *endPt);
void editText(struct LinkedNode *node, struct Vertex *startPt, struct Vertex *endPt,
              char *text = NULL, int fontWidth = -1, int fontHeight = -1);
void drawNodeData(struct NodeData *nodeData, color_t fgColor, bool isDraft);
void redrawAll(struct LinkedList *list, color_t fgColor, bool isDraft);

struct Vertex * getTextEndPt(struct Vertex *startPt, char *text, LOGFONT *font);
void drawText(struct Vertex *startPt, struct Vertex *endPt, char *text,
              LOGFONT *font, color_t textColor, color_t fillColor);

// Tracking functions
struct Vertex * trackEndPt(struct LinkedList *list, struct Vertex *startPt,
                        int shapeType, color_t newFgColor, color_t drawnFgColor);
void trackEditPts(struct LinkedList *list, struct NodeData *data, int assistId,
                struct Vertex **startPt, struct Vertex **endPt);
void trackShape(struct LinkedList *list, struct NodeData *data, struct Vertex *cursorPt,
                struct Vertex **startPt, struct Vertex **endPt);

#endif
