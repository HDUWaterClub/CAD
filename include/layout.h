#ifndef LAYOUT_H_
#define LAYOUT_H_

#include "datatypes.h"

/* Area */

#define AREA_MENU 0
#define AREA_CANVAS 1
#define AREA_ALL 2

/* Button */

// Colors
#define BUTTON_FILL_COLOR_ACTIVE EGERGB(40, 40, 40)
#define BUTTON_FILL_COLOR_INACTIVE EGERGB(56, 56, 56)
#define BUTTON_FOREGROUND_COLOR_ACTIVE EGERGB(255, 255, 255)
#define BUTTON_FOREGROUND_COLOR_INACTIVE EGERGB(245, 245, 245)

// States and types
#define BUTTON_STATE_AVAILABLE 1
#define BUTTON_STATE_ACTIVE 2
#define BUTTON_TYPE_SEGMENT 0
#define BUTTON_TYPE_RECTANGLE 1
#define BUTTON_TYPE_CIRCLE 2
#define BUTTON_TYPE_ELLIPSE 3
#define BUTTON_TYPE_TEXT 4
#define BUTTON_TYPE_CLEAR 5
#define BUTTON_TYPE_EXIT 6
#define BUTTON_NON_ACTIVE 7
#define BUTTON_NON_CHANGED 8

// Number and length
#define BUTTON_NUMBER 7
#define BUTTON_TEXT_LENGTH 127

/* Canvas */

// Color
#define CANVAS_COLOR BLACK

/* Edit Assist */

// Color
#define EDIT_ASSIST_COLOR EGERGB(169, 169, 169)

// Configurations
#define EDIT_ASSIST_DRAW_SIDE_SAME 1
#define EDIT_ASSIST_DRAW_SIDE_NON_SAME 2
#define EDIT_ASSIST_DRAW_MID 4

// Number and radius
#define EDIT_ASSIST_MAX_NUM 8
#define EDIT_ASSIST_RADIUS 5

/* Menu */

// Color
#define MENU_BACKGROUND_COLOR EGERGB(56, 56, 56)

// Width
#define MENU_MIN_WIDTH 240

/* Screen */

// Width and height
#define SCREEN_MIN_WIDTH 640
#define SCREEN_MIN_HEIGHT 480
#define SCREEN_MAX_WIDTH 16384
#define SCREEN_MAX_HEIGHT 16384

struct Button {
    bool isAvailable;
    int minx, miny, width, height;
    char text[BUTTON_TEXT_LENGTH];
};

extern int screenWidth, screenHeight;
extern int menuWidth, canvasWidth;
extern int paddingWidth, paddingHeight;
extern struct Vertex editAssistArr[EDIT_ASSIST_MAX_NUM];

/* Area and view port */
int getViewPort();
void setViewPort(int areaId);
int whichArea(int x);

/* Button */
int getButtonId(int x, int y);
void changeButtonText(int buttonId, char *text);
void drawButton(int buttonId, int state);

/* Canvas */
void clearCanvas();

/* Edit Assist */
bool isInAssistArea(struct Vertex *cursorPt);
int getAssistId(struct Vertex *cursorPt);
void drawEditAssist(struct NodeData *nodeData);

/* Initialization */
void init();

#endif
