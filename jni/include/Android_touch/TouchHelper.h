#ifndef TOUCHHELPER_H
#define TOUCHHELPER_H

struct TouchFinger
{
    int x = 0;
    int y = 0;
    bool isDown = false;
};

void Touch_Up(int s);
void Touch_Init();
void Touch_Down(int s, int x, int y);
void Touch_Move(int s, int x, int y);

#endif
