#include <stdio.h>
#include <limits.h>
#include <assert.h>

#include "windows.h"

#include "datatypes.h"
#include "distance.h"
#include "draw.h"
#include "layout.h"

struct Button btnArr[BUTTON_NUMBER];

struct Vertex editAssistArr[EDIT_ASSIST_MAX_NUM];

int screenWidth, screenHeight;
int menuWidth, canvasWidth;
int paddingWidth, paddingHeight;

LOGFONT buttonFont, logoFont;

void printLog(char *str) {
    MessageBox(NULL, str, "FUCK", MB_OK | MB_SYSTEMMODAL);
}

void getMonitorResolution() {
    screenWidth = GetSystemMetrics(SM_CXSCREEN);
    screenHeight = GetSystemMetrics(SM_CYSCREEN);
}

int getViewPort() {
    int cntCanvasMinx = -1, cntCanvasMiny = -1, cntCanvasMaxx = -1, cntCanvasMaxy = -1;
    getviewport(&cntCanvasMinx, &cntCanvasMaxy, &cntCanvasMaxx, &cntCanvasMiny);
    assert(cntCanvasMinx >= 0 && cntCanvasMiny >= 0 && cntCanvasMaxx >= 0 && cntCanvasMaxy >= 0);

    return cntCanvasMinx > 0 ? AREA_CANVAS : AREA_MENU;
}

void setViewPort(int areaId) {
    if (getViewPort() == areaId) {
        return;
    }
    switch (areaId) {
        case AREA_MENU: {
            setviewport(0, 0, menuWidth, screenHeight, true);
            break;
        }
        case AREA_CANVAS: {
            setviewport(menuWidth + 1, 0, screenWidth, screenHeight, true);
            break;
        }
        default: {
            setviewport(0, 0, screenWidth, screenHeight);
            break;
        }
    }
}

void clearCanvas() {
    int prevViewPort = getViewPort();
    setViewPort(AREA_CANVAS);
    fillBlock(0, 0, canvasWidth, screenHeight, CANVAS_COLOR);
    setViewPort(prevViewPort);
}

bool isInRec(int x, int y, int x1, int y1, int x2, int y2) {
    int minx = min(x1, x2), miny = min(y1, y2);
    int maxx = max(x1, x2), maxy = max(y1, y2);

    return x >= minx && x <= maxx && y >= miny && y <= maxy;
}

int whichArea(int x) {
    return x <= menuWidth ? AREA_MENU : AREA_CANVAS;
}

int getButtonId(int x, int y) {
    for (int i = 0; i < BUTTON_NUMBER; i++) {
        if (!btnArr[i].isAvailable) {
            continue;
        }

        if (isInRec(x, y, btnArr[i].minx, btnArr[i].miny,
            btnArr[i].minx + btnArr[i].width, btnArr[i].miny + btnArr[i].height)) {
                return i;
        }
    }
    return BUTTON_NON_CHANGED;
}

void drawButton(int buttonId, int state) {
    assert(buttonId >= 0 && buttonId < BUTTON_NUMBER);

    int prevViewPort = getViewPort();
    setViewPort(AREA_MENU);

    color_t prevColor = getcolor(), prevFillColor = getfillcolor();

    if (state & BUTTON_STATE_AVAILABLE) {
        btnArr[buttonId].isAvailable = true;
    } else {
        btnArr[buttonId].isAvailable = false;
    }

    color_t textColor = BUTTON_FOREGROUND_COLOR_INACTIVE, fillColor = BUTTON_FILL_COLOR_INACTIVE;
    if (state & BUTTON_STATE_ACTIVE) {
        textColor = BUTTON_FOREGROUND_COLOR_ACTIVE;
        fillColor = BUTTON_FILL_COLOR_ACTIVE;
    }

    setcolor(textColor);
    setfillcolor(fillColor);
    setfontbkcolor(fillColor);

    setfont(&buttonFont);
    int cntTextHeight = textheight(btnArr[buttonId].text);
    int cntTextWidth = textwidth(btnArr[buttonId].text);
    if (cntTextHeight & 1) {
        cntTextHeight++;
    }
    if (cntTextWidth & 1) {
        cntTextWidth++;
    }

    int cntTextPaddingHeight = (btnArr[buttonId].height - cntTextHeight) / 2;
    // int cntTextPaddingWidth = (btnArr[buttonId].width - cntTextWidth) / 2;

    // rectangle(btnArr[buttonId].minx, btnArr[buttonId].miny + btnArr[buttonId].height, btnArr[buttonId].minx + btnArr[buttonId].width, btnArr[buttonId].miny);
    bar(btnArr[buttonId].minx, btnArr[buttonId].miny + btnArr[buttonId].height, btnArr[buttonId].minx + btnArr[buttonId].width, btnArr[buttonId].miny);
    outtextrect(btnArr[buttonId].minx + paddingWidth, btnArr[buttonId].miny + cntTextPaddingHeight, cntTextWidth, cntTextHeight, btnArr[buttonId].text);

    setcolor(prevColor);
    setfillcolor(prevFillColor);
    setViewPort(prevViewPort);
}

void changeButtonText(int buttonId, char text[BUTTON_TEXT_LENGTH]) {
    assert(buttonId >= 0 && buttonId < BUTTON_NUMBER && text != NULL);
    strncpy(btnArr[buttonId].text, text, sizeof(btnArr[buttonId].text));
}

void drawLogo(int minx, int miny, color_t fillColor, color_t textColor) {
    int prevViewPort = getViewPort();
    setViewPort(AREA_MENU);

    color_t prevColor = getcolor(), prevFillColor = getfillcolor();

    setcolor(textColor);
    setfillcolor(fillColor);
    setfontbkcolor(fillColor);

    setfont(&logoFont);
    outtextxy(minx, miny, "CADET");

    setcolor(prevColor);
    setfillcolor(prevFillColor);

    setViewPort(prevViewPort);
}

void drawMenu() {
    int prevViewPort = getViewPort();
    setViewPort(AREA_MENU);

    fillBlock(0, 0, menuWidth, screenHeight, MENU_BACKGROUND_COLOR);

    int buttonWidth = menuWidth - (paddingWidth << 1);
    int buttonHeight = screenHeight / 10 - (paddingHeight << 1);
    if (buttonWidth & 1) {
        buttonWidth++;
    }
    if (buttonHeight & 1) {
        buttonHeight++;
    }

    int logoFontHeight = screenHeight / 10 - (paddingHeight << 1);
    if (logoFontHeight & 1) {
        logoFontHeight--;
    }

    logoFont.lfWidth = 0;
    logoFont.lfHeight = logoFontHeight;
    logoFont.lfWeight = FW_SEMIBOLD;
    logoFont.lfQuality = PROOF_QUALITY;

    drawLogo(paddingWidth, screenHeight / 10, BUTTON_FILL_COLOR_INACTIVE, BUTTON_FOREGROUND_COLOR_INACTIVE);

    int buttonFontHeight = buttonHeight - (paddingHeight * 3 / 2);
    if (buttonFontHeight & 1) {
        buttonFontHeight--;
    }

    buttonFont.lfWidth = 0;
    buttonFont.lfHeight = buttonFontHeight;
    buttonFont.lfWeight = FW_MEDIUM;
    buttonFont.lfQuality = PROOF_QUALITY;

    snprintf(btnArr[0].text, sizeof(btnArr[0].text), "SEGMENT");
    snprintf(btnArr[1].text, sizeof(btnArr[1].text), "RECTANGLE");
    snprintf(btnArr[2].text, sizeof(btnArr[2].text), "CIRCLE");
    snprintf(btnArr[3].text, sizeof(btnArr[3].text), "ELLIPSE");
    snprintf(btnArr[4].text, sizeof(btnArr[4].text), "TEXT");
    snprintf(btnArr[5].text, sizeof(btnArr[5].text), "CLEAR");
    snprintf(btnArr[6].text, sizeof(btnArr[6].text), "EXIT");

    for (int i = 0; i < BUTTON_NUMBER; i++) {
        btnArr[i].isAvailable = true;
        btnArr[i].minx = paddingWidth;
        btnArr[i].miny = screenHeight * (i + 2) / 10 + paddingHeight;
        btnArr[i].width = buttonWidth;
        btnArr[i].height = buttonHeight;

        if (btnArr[i].isAvailable) {
            drawButton(i, BUTTON_STATE_AVAILABLE);
        }
    }

    setViewPort(prevViewPort);
}

void init() {
    getMonitorResolution();
    if (screenWidth < SCREEN_MIN_WIDTH || screenWidth > SCREEN_MAX_WIDTH || screenHeight < SCREEN_MIN_HEIGHT || screenHeight > SCREEN_MAX_HEIGHT) {
        puts("Unsupported screen resolution");
        exit(1);
    }
    setinitmode(INIT_NOBORDER, 0, 0);
    initgraph(screenWidth, screenHeight);

     // Boring calculations
    menuWidth = screenWidth / 4;
    menuWidth = min(menuWidth, MENU_MAX_WIDTH);
    menuWidth = max(menuWidth, MENU_MIN_WIDTH);
    if (menuWidth & 1) {
        if (screenWidth & 3) {
            menuWidth++;
        } else {
            menuWidth--;
        }
    }
    canvasWidth = screenWidth - menuWidth;
    paddingWidth = menuWidth / 10, paddingHeight = screenHeight / 50;

    drawMenu();

    defaultFont.lfWidth = 0;
    defaultFont.lfHeight = buttonFont.lfHeight;
    defaultFont.lfWeight = FW_REGULAR;
    defaultFont.lfQuality = PROOF_QUALITY;
    setfont(&defaultFont);

    // Set viewport to canvas
    clearCanvas();
    setViewPort(AREA_CANVAS);
}

void drawEditAssist(struct NodeData *nodeData) {
    assert(nodeData != NULL);
    color_t prevFillColor = getfillcolor();
    setfillcolor(EDIT_ASSIST_COLOR);

    int minx, miny, maxx, maxy;
    int drawConf = 0;

    switch (nodeData -> type) {
        case DATATYPE_SEGMENT: {
            struct Segment *seg = (struct Segment *)nodeData -> content;
            minx = seg -> leftPt -> x, miny = seg -> leftPt -> y;
            maxx = seg -> rightPt -> x, maxy = seg -> rightPt -> y;
            drawConf = EDIT_ASSIST_DRAW_SIDE_SAME;
            break;
        }
        case DATATYPE_RECTANGLE: {
            struct Rectangle *rec = (struct Rectangle *)nodeData -> content;
            minx = rec -> lowerLeftPt -> x, miny = rec -> lowerLeftPt -> y;
            maxx = rec -> upperRightPt -> x, maxy = rec -> upperRightPt -> y;
            drawConf = EDIT_ASSIST_DRAW_SIDE_SAME + EDIT_ASSIST_DRAW_SIDE_NON_SAME + EDIT_ASSIST_DRAW_MID;
            break;
        }
        case DATATYPE_CIRCLE: {
            struct Circle *cir = (struct Circle *)nodeData -> content;
            minx = cir -> centerPt -> x - cir -> radius, miny = cir -> centerPt -> y - cir -> radius;
            maxx = cir -> centerPt -> x + cir -> radius, maxy = cir -> centerPt -> y + cir -> radius;
            drawConf = EDIT_ASSIST_DRAW_MID;
            break;
        }
        case DATATYPE_ELLIPSE: {
            struct Ellipse * elp = (struct Ellipse *)nodeData -> content;
            minx = elp -> centerPt -> x - elp -> majorSemiAxis, miny = elp -> centerPt -> y - elp -> minorSemiAxis;
            maxx = elp -> centerPt -> x + elp -> majorSemiAxis, maxy = elp -> centerPt -> y + elp -> minorSemiAxis;
            drawConf = EDIT_ASSIST_DRAW_SIDE_SAME + EDIT_ASSIST_DRAW_SIDE_NON_SAME + EDIT_ASSIST_DRAW_MID;
            break;
        }
        case DATATYPE_TEXT: {
            struct Text *txt = (struct Text *)nodeData -> content;
            struct Rectangle *pos = (struct Rectangle *)txt -> position;
            minx = pos -> lowerLeftPt -> x, miny = pos -> lowerLeftPt -> y;
            maxx = pos -> upperRightPt -> x, maxy = pos -> upperRightPt -> y;
            drawConf = EDIT_ASSIST_DRAW_SIDE_SAME + EDIT_ASSIST_DRAW_SIDE_NON_SAME;
            break;
        }
        default: {
            break;
        }
    }

    // Initialize
    for (int i = 0; i < EDIT_ASSIST_MAX_NUM; i++) {
        editAssistArr[i].x = -1;
        editAssistArr[i].y = -1;
    }

    for (int i = 0; i < EDIT_ASSIST_MAX_NUM; i++) {
        const int xType = i / 3, yType = i % 3;
        int cntx, cnty;

        // Validate
        if (!(drawConf & EDIT_ASSIST_DRAW_SIDE_SAME)) {
            if (xType != 2 && yType != 2 && xType == yType) {
                continue;
            }
        }
        if (!(drawConf & EDIT_ASSIST_DRAW_SIDE_NON_SAME)) {
            if (xType != 2 && yType != 2 && xType != yType) {
                continue;
            }
        }
        if (!(drawConf & EDIT_ASSIST_DRAW_MID)) {
            if (xType == 2 || yType == 2) {
                continue;
            }
        }

        if (xType == 0) {
            cntx = minx;
        } else if (xType == 1) {
            cntx = maxx;
        } else {
            cntx = (minx + maxx) / 2;
        }

        if (yType == 0) {
            cnty = miny;
        } else if (yType == 1) {
            cnty = maxy;
        } else {
            cnty = (miny + maxy) / 2;
        }

        editAssistArr[i].x = cntx;
        editAssistArr[i].y = cnty;

        if (cntx >= 0 && cnty >= 0) {
            fillellipse(cntx, cnty, EDIT_ASSIST_RADIUS, EDIT_ASSIST_RADIUS);
        }
    }

    setcolor(prevFillColor);
}

bool isInAssistArea(struct Vertex *cursorPt) {
    assert(cursorPt != NULL);
    for (int i = 0; i < EDIT_ASSIST_MAX_NUM; i++) {
        if (editAssistArr[i].x >= 0 && editAssistArr[i].y >= 0) {
            struct Circle *cir = makeCircle(&editAssistArr[i], EDIT_ASSIST_RADIUS);
            bool res = findCircleRule(cursorPt, cir);
            destoryCircle(cir);
            if (res) {
                return res;
            }
        }
    }
    return false;
}

void getStartEndPts(struct NodeData *data, struct Vertex **startPt, struct Vertex **endPt, int assistId) {
    assert(data != NULL && *startPt == NULL && *endPt == NULL);

    if (assistId == -1) {
        if (data -> type == DATATYPE_CIRCLE) {
            assistId = 2;
        } else {
            assistId = 0;
        }
    }

    const int xType = assistId / 3, yType = assistId % 3;
    int startPtx, startPty;
    int endPtx = editAssistArr[assistId].x, endPty = editAssistArr[assistId].y;

    switch (data -> type) {
        case DATATYPE_SEGMENT: {
            struct Segment *seg = (struct Segment *)data -> content;
            if (xType == 0) {
                startPtx = seg -> rightPt -> x;
                startPty = seg -> rightPt -> y;
            } else {
                startPtx = seg -> leftPt -> x;
                startPty = seg -> leftPt -> y;
            }
            break;
        }
        case DATATYPE_RECTANGLE: {
            struct Rectangle *rec = (struct Rectangle *)data -> content;

            if (xType == 0) {
                startPtx = rec -> upperRightPt -> x;
            } else {
                startPtx = rec -> lowerLeftPt -> x;
                if (xType == 2) {
                    endPtx = rec -> upperRightPt -> x;
                }
            }
            if (yType == 0) {
                startPty = rec -> upperRightPt -> y;
            } else {
                startPty = rec -> lowerLeftPt -> y;
                if (yType == 2) {
                    endPty = rec -> upperRightPt -> y;
                }
            }
            break;
        }
        case DATATYPE_CIRCLE: {
            struct Circle *cir = (struct Circle *)data -> content;

            if (xType == 2) {
                startPtx = endPtx;
                if (yType == 0) {
                    startPty = endPty + cir -> radius * 2;
                } else {
                    startPty = endPty - cir -> radius * 2;
                }
            }
            if (yType == 2) {
                if (xType == 0) {
                    startPtx = endPtx + cir -> radius * 2;
                } else {
                    startPtx = endPtx - cir -> radius * 2;
                }
                startPty = endPty;
            }
            break;
        }
        case DATATYPE_ELLIPSE: {
            struct Ellipse *elp = (struct Ellipse *)data -> content;

            if (xType == 0) {
                startPtx = elp -> centerPt -> x + elp -> majorSemiAxis;
            } else {
                startPtx = elp -> centerPt -> x - elp -> majorSemiAxis;
                if (xType == 2) {
                    endPtx = elp -> centerPt -> x + elp -> majorSemiAxis;
                }
            }
            if (yType == 0) {
                startPty = elp -> centerPt -> y + elp -> minorSemiAxis;
            } else {
                startPty = elp -> centerPt -> y - elp -> minorSemiAxis;
                if (yType == 2) {
                    endPty = elp -> centerPt -> y + elp -> minorSemiAxis;
                }
            }

            break;
        }
        case DATATYPE_TEXT: {
            struct Text *txt = (struct Text *)data -> content;
            struct Rectangle *pos = (struct Rectangle *)txt -> position;

            if (xType == 0) {
                startPtx = pos -> upperRightPt -> x;
            } else {
                startPtx = pos -> lowerLeftPt -> x;
                if (xType == 2) {
                    endPtx = pos -> upperRightPt -> x;
                }
            }
            if (yType == 0) {
                startPty = pos -> upperRightPt -> y;
            } else {
                startPty = pos -> lowerLeftPt -> y;
                if (yType == 2) {
                    endPty = pos -> upperRightPt -> y;
                }
            }
            break;
        }
        default: {
            return;
        }
    }

    *startPt = makeVertex(startPtx, startPty);
    *endPt = makeVertex(endPtx, endPty);
}

int getAssistId(struct Vertex *cursorPt) {
    int assistId = -1, minManhattanDistance = INT_MAX;
    for (int i = 0; i < EDIT_ASSIST_MAX_NUM; i++) {
        if (editAssistArr[i].x >= 0 && editAssistArr[i].y >= 0) {
            int cntManhattanDistance = getManhattanDistance(cursorPt, &editAssistArr[i]);
            if (cntManhattanDistance <= FINDRULE_VARIATION && cntManhattanDistance <= minManhattanDistance) {
                minManhattanDistance = cntManhattanDistance;
                assistId = i;
            }
        }
    }
    return assistId;
}
