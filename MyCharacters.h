#pragma once
#include "MyDirectX.h"

class MyCharacters
{
public:
	MyCharacters()
	{
		Grav_Velc = Grav_time = 0;
		velx = vely = 0.0f;
		X_point = Y_point = 0;
	};

	~MyCharacters();

	//time variables
	int starttime, Grav_time, delay, time, TimeCount;
	
	//Physics variables
	float X_point, Y_point;
	float Grav_Velc, Grav_Acc;
	float velx, vely;
	bool Collision_Type[3] = { 0,0,0 };
	bool Up_Down = false;
	bool jumping = false;
	D3DCOLOR color;

	void Platform_Collisions(SPRITE character, SPRITE Platforms[]);

	// physics functions
	void Time();

	void Gravity();

	void Dash(SPRITE character);

	void Jump(SPRITE &character, int Start_F, int End_F, int Suspension_F);
};

