#include "ChronoDisplay.hpp"

ChronoDisplay::ChronoDisplay(u32 color) {
    this->color = color;
    this->g_dynamicBuf = C2D_TextBufNew(128);
}

ChronoDisplay::~ChronoDisplay() {
    delete &g_dynamicBuf;
    delete &color;
}

void ChronoDisplay::Draw(C3D_RenderTarget* target, int w, int h) {
    C2D_TextBufClear(g_dynamicBuf);
    C2D_Text dynText;
    C2D_TextParse(&dynText, g_dynamicBuf, chronoText);
    C2D_TextOptimize(&dynText);
    C2D_Prepare();
    C2D_DrawText(&dynText, C2D_AlignCenter | C2D_WithColor, w / 2, 8.0f, 1.0f, 1.0f, 1.0f, color);
    C2D_Flush();
}

void ChronoDisplay::SetElapsedTime(Chronometer* chronometer) {
    chronometer->GetElapsedTimeInStringFormat(chronoText);
}