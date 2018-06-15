#ifndef DRAW_H_
#define DRAW_H_

#include "graphics.h"

#include "linkedlist.h"
#include "datatypes.h"

#define SHAPE_DEFAULT_COLOR WHITE

void fillBlock(int minx, int maxy, int maxx, int miny, int fillColor);

void getRealPosition(struct Vertex *cntPt);

void drawShape(struct Vertex *startPt, struct Vertex *endPt, int shapeType, color_t fgColor);
struct LinkedNode *saveShape(struct LinkedList *list, struct Vertex *startPt, struct Vertex *endPt, int shapeType);
void editShape(struct LinkedNode *node, struct Vertex *startPt, struct Vertex *endPt);
void drawNodeData(struct NodeData *nodeData, color_t fgColor);
void redrawAll(struct LinkedList *list, color_t fgColor);

struct Vertex * trackEndPt(struct LinkedList *list, struct Vertex *startPt,
                        int shapeType, color_t newFgColor, color_t drawnFgColor);
void trackEditPts(struct LinkedList *list, struct NodeData *data, int assistId,
                struct Vertex **startPt, struct Vertex **endPt);

void trackShape(struct LinkedList *list, struct NodeData *data, struct Vertex *cursorPt,
                struct Vertex **startPt, struct Vertex **endPt);

#endif
