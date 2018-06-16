#ifndef LAYOUT_H_
#define LAYOUT_H_

#include "draw.h"

// Area
#define AREA_MENU 0
#define AREA_CANVAS 1

// Button
#define BUTTON_NUMBER 7
#define BUTTON_TEXT_LENGTH 127

#define BUTTON_STATE_INACTIVE 0
#define BUTTON_STATE_ACTIVE 1

#define BUTTON_FILL_COLOR_ACTIVE EGERGB(40, 40, 40)
#define BUTTON_FILL_COLOR_INACTIVE EGERGB(56, 56, 56)
#define BUTTON_FOREGROUND_COLOR_ACTIVE EGERGB(255, 255, 255)
#define BUTTON_FOREGROUND_COLOR_INACTIVE EGERGB(245, 245, 245)

#define BUTTON_TYPE_SEGMENT 0
#define BUTTON_TYPE_RECTANGLE 1
#define BUTTON_TYPE_CIRCLE 2
#define BUTTON_TYPE_ELLIPSE 3
#define BUTTON_TYPE_TEXT 4
#define BUTTON_TYPE_CLEAR 5
#define BUTTON_TYPE_EXIT 6
#define BUTTON_NON_ACTIVE 7
#define BUTTON_NON_CHANGED 8

// Width and height
#define SCREEN_MIN_WIDTH 640
#define SCREEN_MIN_HEIGHT 480

#define MENU_MIN_WIDTH 240
#define MENU_MAX_WIDTH 360
#define MENU_BACKGROUND_COLOR EGERGB(56, 56, 56)

#define EDIT_ASSIST_MAX_NUM 8
#define EDIT_ASSIST_COLOR EGERGB(169, 169, 169)
#define EDIT_ASSIST_RADIUS 5

#define EDIT_ASSIST_MAX_NUM 8

#define EDIT_ASSIST_DRAW_SIDE_SAME 1
#define EDIT_ASSIST_DRAW_SIDE_NON_SAME 2
#define EDIT_ASSIST_DRAW_MID 4


struct Button {
    bool isAvailable;
    int minx, miny, width, height;
    char text[BUTTON_TEXT_LENGTH];
};

extern int screenWidth, screenHeight;
extern int menuWidth, canvasWidth;
extern struct Vertex editAssistArr[EDIT_ASSIST_MAX_NUM];

void printLog(char *str);

void clearCanvas();

int getViewPort();
void setViewPort(int areaId);

bool isInRec(int x, int y, int x1, int y1, int x2, int y2);
int whichArea(int x);
int getButtonId(int x, int y);

void drawButton(int buttonId, bool state);
void changeButtonText(int buttonId, char *text);

void init();

void drawEditAssist(struct NodeData *nodeData);
bool isInAssistArea(struct Vertex *cursorPt);
void getStartEndPts(struct NodeData *data, struct Vertex **startPt, struct Vertex **endPt, int assistId = -1);

int getAssistId(struct Vertex *cursorPt);

#endif
