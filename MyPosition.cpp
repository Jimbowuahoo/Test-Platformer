#include "MyPosition.h" 

void Position(MyCharacters Main, SPRITE MainChar, const int BUFFERH, const int BUFFERW, double Scroll_X, double Scroll_Y)
{
	if (MainChar.x < SCREENW*0.4 || MainChar.x > SCREENW*0.6)
	{
		Scroll_X = MainChar.Pos_X;
	}
	else
	{
		MainChar.x = MainChar.Pos_X;
	}
	
	if (MainChar.x < SCREENH*0.2 || MainChar.x > SCREENH*0.5)
	{
		Scroll_Y = MainChar.Pos_Y;
	}
	else
	{
		MainChar.y = SCREENH - MainChar.Pos_Y;
	}
	

	if (Scroll_Y < 0)
		Scroll_Y = BUFFERH - SCREENH;
	if (Scroll_Y > BUFFERH - SCREENH)
		Scroll_Y = 0;
	if (Scroll_X < 0)
		Scroll_X = BUFFERW - SCREENW;
	if (Scroll_X > BUFFERW - SCREENW)
		Scroll_X = 0;
}