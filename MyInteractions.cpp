#include "MyDirectX.h"
using namespace std;

int DashTime = 0;
float Y_velc;

void Impulse_dash(SPRITE sprite)
{
	if ((int)GetTickCount > sprite.starttime + 25)
	{
		sprite.starttime = GetTickCount();
		DashTime += 1;
	}
	Y_velc = DashTime*DashTime*0.000001;
}

bool CollisionR(SPRITE sprite1, SPRITE sprite2)
{
	double radius1, radius2;

	//calculate radius 1
	if (sprite1.width > sprite1.height)
		radius1 = (sprite1.width*sprite1.scalingX) / 2.0;
	else
		radius1 = (sprite1.height*sprite1.scalingY) / 2.0;

	//center point 1
	double x1 = sprite1.x + radius1;
	double y1 = sprite1.y + radius1;
	D3DXVECTOR2 vector1(x1, y1);

	//calculate radius 2
	if (sprite2.width > sprite2.height)
		radius2 = (sprite2.width*sprite2.scalingX) / 2.0;
	else
		radius2 = (sprite2.height*sprite2.scalingY) / 2.0;

	//center point 1
	double x2 = sprite2.x + radius2;
	double y2 = sprite2.y + radius2;
	D3DXVECTOR2 vector2(x2, y2);

	//calculate the distance
	double deltax = vector1.x - vector2.x;
	double deltay = vector1.y - vector2.y;
	double dist = sqrt((deltax*deltax) + (deltay*deltay));

	//return distance comparison
	return(dist < radius1 + radius2);

}

//bounding box collision
int Collision(SPRITE sprite1, SPRITE sprite2)
{
	int Collision_Type = 0;

	RECT Obj1_Left;
	RECT Obj1_Right;
	RECT Obj1_Top;
	RECT Obj1_Bottom;
	if (sprite1.scalingX > 0)
	{
		//Left-side Rectangle
		Obj1_Left.left = (long)sprite1.x - sprite1.lBound;
		Obj1_Left.top = (long)sprite1.y - sprite1.tBound + 5;
		Obj1_Left.right = (long)sprite1.x - sprite1.lBound - 5;
		Obj1_Left.bottom = (long)sprite1.y + sprite1.bBound*sprite1.scalingY - 5;

		//Right-Side Rectangle
		Obj1_Right.left = sprite1.x + sprite1.rBound*sprite1.scalingX - 5;
		Obj1_Right.right = sprite1.x + sprite1.rBound*sprite1.scalingX;
		Obj1_Right.top = (long)sprite1.y - sprite1.tBound + 5;
		Obj1_Right.bottom = (long)sprite1.y + sprite1.bBound*sprite1.scalingY - 5;
	}
	else if (sprite1.scalingX < 0)
	{
		//Left-side Rectangle
		Obj1_Right.left = (long)sprite1.x - sprite1.lBound - 5;
		Obj1_Right.top = (long)sprite1.y - sprite1.tBound + 5;
		Obj1_Right.right = (long)sprite1.x - sprite1.lBound;
		Obj1_Right.bottom = (long)sprite1.y + sprite1.bBound*sprite1.scalingY - 5;

		//Right-Side Rectangle
		Obj1_Left.left = sprite1.x + sprite1.rBound*sprite1.scalingX;
		Obj1_Left.right = sprite1.x + sprite1.rBound*sprite1.scalingX + 5;
		Obj1_Left.top = (long)sprite1.y - sprite1.tBound + 5;
		Obj1_Left.bottom = (long)sprite1.y + sprite1.bBound*sprite1.scalingY - 5;
	}
	//Top Rectangle
	Obj1_Top.top = (long)sprite1.y - sprite1.tBound;
	Obj1_Top.bottom = (long)sprite1.y - sprite1.tBound + 5;
	Obj1_Top.right = (long)sprite1.x + sprite1.rBound - 5;
	Obj1_Top.left = (long)sprite1.x - sprite1.lBound + 5;

	//Bottom Rectangle
	Obj1_Bottom.top = (long)sprite1.y + sprite1.bBound*sprite1.scalingY - 5;
	Obj1_Bottom.bottom = (long)sprite1.y + sprite1.bBound*sprite1.scalingY;
	if (sprite1.scalingX > 0)
	{
		Obj1_Bottom.right = (long)sprite1.x + sprite1.rBound - 5;
		Obj1_Bottom.left = (long)sprite1.x + 5;
	}
	if (sprite1.scalingX < 0)
	{
		Obj1_Bottom.right = (long)sprite1.x - 5;
		Obj1_Bottom.left = (long)sprite1.x - sprite1.lBound + 5;
	}
	

	RECT Obj2_Left;
	RECT Obj2_Right;
	RECT Obj2_Top;
	RECT Obj2_Bottom;
	//Left-side Rectangle
	Obj2_Left.left = (long)sprite2.x - sprite2.lBound;
	Obj2_Left.top = (long)sprite2.y - sprite2.tBound + 5;
	Obj2_Left.right = (long)sprite2.x - sprite2.lBound - 5;
	Obj2_Left.bottom = (long)sprite2.y + sprite2.bBound*sprite1.scalingY - 5;

	//Right-Side Rectangle
	Obj2_Right.left = sprite2.x + sprite2.rBound*sprite2.scalingX - 5;
	Obj2_Right.right = sprite2.x + sprite2.rBound*sprite2.scalingX;
	Obj2_Right.top = (long)sprite2.y - sprite2.tBound + 5;
	Obj2_Right.bottom = (long)sprite2.y + sprite2.bBound*sprite2.scalingY - 5;

	//Top Rectangle
	Obj2_Top.top = (long)sprite2.y - sprite2.tBound;
	Obj2_Top.bottom = (long)sprite2.y - sprite2.tBound + 5;
	Obj2_Top.right = (long)sprite2.x + sprite2.rBound*sprite2.scalingX - 5;
	Obj2_Top.left = (long)sprite2.x - sprite2.lBound + 5;

	//Bottom Rectangle
	Obj2_Bottom.top = (long)sprite2.y + sprite2.bBound*sprite2.scalingY - 5;
	Obj2_Bottom.bottom = (long)sprite2.y + sprite2.bBound*sprite2.scalingY;
	Obj2_Bottom.right = (long)sprite2.x + sprite2.rBound*sprite2.scalingX - 5;
	Obj2_Bottom.left = (long)sprite2.x - sprite2.lBound + 5;

	RECT dest;//ignored

	if (IntersectRect(&dest, &Obj1_Right, &Obj2_Left) == true || IntersectRect(&dest, &Obj1_Left, &Obj2_Right) == true)
	{
		Collision_Type = 2;
	}
	if (IntersectRect(&dest, &Obj1_Top, &Obj2_Bottom) == true || IntersectRect(&dest, &Obj1_Bottom, &Obj2_Top) == true)
	{
		Collision_Type = 1;
	}
	
	return Collision_Type;
}