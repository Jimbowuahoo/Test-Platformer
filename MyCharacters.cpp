#include "MyCharacters.h"


MyCharacters::~MyCharacters()
{
}

void MyCharacters::Time()
{
	if ((int)GetTickCount > time + 500)
	{
		time = GetTickCount();
		Grav_time += 1;
	}
}


void MyCharacters::Gravity()
{
	Time();
	Grav_Velc = Grav_time*0.6;
	Grav_Acc = Grav_time*Grav_time*0.0008;

	if (Grav_Acc > Grav_Velc * 2)
	{
		Grav_Acc = Grav_Velc * 2;
	}
}

void MyCharacters::Platform_Collisions(SPRITE Character, SPRITE Platforms[])
{
	for (int i = 0; i < sizeof(Platforms)/2; i++)
	{
		if (Collision(Character, Platforms[i]) == 1)
		{
			Collision_Type[i] = 1;
		}
		else if (Collision(Character, Platforms[i]) == 2)
		{
			Collision_Type[i] = 2;
		}
		else
		{
			Collision_Type[i] = 0;
		}
	}
	Up_Down = 0;
	for (int j = 0; j < sizeof(Platforms) / 2; j++)
	{
		Up_Down += Collision_Type[j];
	}
}

void MyCharacters::Jump(SPRITE &character, int Start_F, int End_F, int Suspension_F)
{
	//character.y += Grav_Velc;
	if (Up_Down == 0 && character.frame < Start_F || Up_Down == 0 && character.frame > End_F)
	{
		character.frame = Suspension_F;
	}
	if (Up_Down == 1)
	{
		Grav_time = 0;
		Y_point = character.y;
		jumping = false;
	}
	if (character.frame <= End_F && character.frame > Start_F && Up_Down == 0 && jumping == true)
	{
		character.y = Y_point - Grav_Velc + Grav_Acc;
	}
	if (character.frame <= End_F && character.frame > Start_F && Up_Down == 0 && jumping == false)
	{
		character.y = Y_point + Grav_Acc;
	}
	if (character.frame < End_F-1 && character.frame > Start_F && Up_Down == 1)
	{
		character.y -= 0.01;
		jumping = true;
	}
}



void MyCharacters::Dash(SPRITE character)
{
	TimeCount = 0;
	Time();
	//velx += character.scalingX*TimeCount*0.4 + character.scalingX*TimeCount*TimeCount*(-0.0000001);
	if (velx == 0)
	{
		TimeCount = 0;
	}
}
