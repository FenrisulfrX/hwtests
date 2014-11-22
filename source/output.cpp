#include "output.h"

#include <string>
#include <cstring>
#include <cmath>
#include <cstdarg>
#include <cstdio>

#include <3ds.h>

#include "text.h"

static std::string bufferTop;
static std::string bufferBottom;

static int countLines(const std::string& str)
{
    if (str.empty())
    	return 0;

    int cnt = 1;
    for (const char& c : str) {
        if (c == '\n') cnt++;
    }
    
    return cnt;
}

static void deleteFirstLine(std::string* str)
{
    if (str->empty())
    	return;
    
    size_t linebreak = str->find_first_of('\n');
    
    if (linebreak == std::string::npos || linebreak + 1 > str->length()) {
        *str = {};
        return;
    }
    
    *str = str->substr(linebreak + 1);
}

static void drawFrame(gfxScreen_t screen, u8 b, u8 g, u8 r)
{
    int screenWidth;
    std::string textBuffer;
    if (screen == GFX_TOP) {
    	screenWidth = 400;
    	textBuffer = bufferTop;
    } else {
    	screenWidth = 320;
    	textBuffer = bufferBottom;
    }

    // Fill screen with color (TODO: find more efficient way to do this)
    u8* bufAdr = gfxGetFramebuffer(screen, GFX_LEFT, NULL, NULL);
    for (int i = 0; i < screenWidth * 240 * 3; i += 3) {
    	bufAdr[i]   = b;
    	bufAdr[i+1] = g;
    	bufAdr[i+2] = r;
    }

    int lines = countLines(textBuffer);
    while (lines > (240 / fontDefault.height - 3)) {
    	deleteFirstLine(&textBuffer);
    	lines--;
    }
    gfxDrawText(screen, GFX_LEFT, NULL, textBuffer.c_str(), 240 - fontDefault.height * 3, 10);
}

void drawFrames()
{
    drawFrame(GFX_TOP, 0x88, 0x66, 0x00);
    drawFrame(GFX_BOTTOM, 0x00, 0x00, 0x00);
    gfxFlushBuffers();
    gfxSwapBuffers();
}

void print(gfxScreen_t screen, const char* format, ...)
{
    std::string& textBuffer = (screen == GFX_TOP) ? bufferTop : bufferBottom;
    va_list arguments;
    char newStr[512];

    va_start(arguments, format);
    vsprintf(newStr, format, arguments);
    va_end(arguments);

    textBuffer += newStr;
    svcOutputDebugString(newStr, strlen(newStr));

    drawFrames();
}

void clearScreen(gfxScreen_t screen)
{
    std::string& textBuffer = (screen == GFX_TOP) ? bufferTop : bufferBottom;
    textBuffer.clear();
    drawFrames();
}

void clearScreens()
{
    clearScreen(GFX_TOP);
    clearScreen(GFX_BOTTOM);
}
