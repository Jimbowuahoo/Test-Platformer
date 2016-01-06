#include "MyDirectX.h"
#include "Fmod.h"
#include "MyInput.h"
#include "MyCharacters.h"

const string APPTITLE = "Zero";
const int SCREENW = 1024;
const int SCREENH = 768;

const int BUFFERW = 4098 * 2;
const int BUFFERH = 768 * 2;

LPDIRECT3DSURFACE9 background = NULL;
double scrollx = 0, scrolly = 0;

// Character animation variables
int CharWait = 0;

cInputDevices input1;

//Variable for the font
LPD3DXFONT font;

//sprite definitions
SPRITE MainChar;
SPRITE Enemy;
SPRITE *Platforms = new SPRITE[2];

//Define classes
MyCharacters Main;

//Sprite variables for the images
LPDIRECT3DTEXTURE9 imgFloor = NULL;

LPDIRECT3DTEXTURE9 imgMain = NULL;

LPDIRECT3DTEXTURE9 imgEnemy = NULL;

LPDIRECT3DTEXTURE9 *imgPlat = new LPDIRECT3DTEXTURE9[2];


//Player Directions
enum PLAYER_DIRECTIONS
{
	NONE = 0,
	LEFT = 1,
	RIGHT = 2
};

//Initial player states
PLAYER_DIRECTIONS pDirection = RIGHT;
PLAYER_STATES playerS = STANDING;

//Definitions for the Fmod sound system
FMOD::System		*fmodsystem;
FMOD::Sound			*music, *effect1;
FMOD::Channel		*channel = 0;
unsigned int		version;

//Functions for the character animations, can be reused if the sprite sheets are normalized
void State_Animate()
{
	if (MainChar.frame == 18)
	{
		MainChar.width = 200;
		MainChar.columns = 8;
	}
	if (MainChar.frame == 23 && CharWait == 1)
	{
		CharWait = 0;
		MainChar.frame = 32;
		MainChar.width = 100;
		MainChar.columns = 16;
	}
	if (MainChar.frame == MainChar.endframe+1)
	{
		CharWait = 0;
	}
	if (playerS == STANDING && CharWait == 0)
	{
		MainChar.width = 100;
		MainChar.columns = 16;
		MainChar.delay = 150;
		MainChar.startframe = 32;
		MainChar.endframe = 34;
		MainChar.direction = 1;
	}
	if (playerS == RUNNING && CharWait == 0)
	{
		MainChar.delay = 50;
		MainChar.startframe = 2;
		MainChar.endframe = 14;
	}

	if (playerS == LIGHT_ATK && CharWait == 0) 
	{
		MainChar.delay = 15;
		CharWait = 1;
		MainChar.startframe = 48;
		MainChar.endframe = 58;
	}

	if (playerS == JUMPING && CharWait == 0)
	{
		MainChar.delay = 50;
		CharWait = 2;
		MainChar.startframe = 64;
		MainChar.endframe = 73;

	}
	if (playerS == DASHING && CharWait == 0)
	{
		MainChar.delay = 60;
		CharWait = 1;
		MainChar.startframe = 18;
		MainChar.endframe = 22;
	}

	Sprite_Animate(MainChar.frame, MainChar.startframe, MainChar.endframe, MainChar.direction, MainChar.starttime, MainChar.delay);
}

//game initialization function
bool Game_Init(HWND window)
{
	FMOD_RESULT			result;
	result = FMOD::System_Create(&fmodsystem);
	result = fmodsystem ->init(50, FMOD_INIT_NORMAL, 0);
	result = fmodsystem->createSound("assets/swish.wav", FMOD_DEFAULT, 0, &effect1);
	result = fmodsystem->createStream("assets/Music.wav", FMOD_DEFAULT, 0, &music);
	
	//initializing the input system
	input1.GetData(WM_INPUT);

	//Initializing direct3D
	if (!Direct3D_Init(window, SCREENW, SCREENH, FALSE))
	{
		MessageBox(0, "Error initializing Direct3D", "ERROR", 0);
		return false;
	}

	//This is the Input system from the directx SDK, not currently in use. Probably will be deleted in the future
	if (!DirectInput_Init(window))
	{
		MessageBox(0, "Error initializing DirectInput", "ERROR", 0);
		return false;
	}

	//load the sprite textures
	imgMain = LoadTexture("zero.png");
	if (!imgMain)return false;

	imgPlat[1] = LoadTexture("assets/CityFloor.png");
	if (!imgPlat[1])return false;

	imgEnemy = LoadTexture("HelmedHorror.png");
	if (!imgEnemy)return false;

	imgPlat[0] = LoadTexture("assets/platform.png");
	if (!imgPlat[0])return false;

	//load the background
	LPDIRECT3DSURFACE9 image = NULL;
	image = LoadSurface("assets/City.jpg");
	if (!image) return false;

	//set properties for sprites
	MainChar.startframe = 0;
	MainChar.endframe = 0;
	MainChar.width = MainChar.height = 100;
	MainChar.x = (SCREENW-MainChar.width)/2;
	MainChar.columns = 16;
	MainChar.rBound = 50;
	MainChar.lBound = 40;
	MainChar.bBound = 70;

	Platforms[1].width = 4098;
	Platforms[1].height = 193;
	Platforms[1].x = 0;
	Platforms[1].y = SCREENH - 193;
	Platforms[1].rBound = 4098;
	Platforms[1].bBound = 193;

	Platforms[0].width = 250;
	Platforms[0].height = 20;
	Platforms[0].x = 250;
	Platforms[0].y = 500;
	Platforms[0].rBound = 250;
	Platforms[0].bBound = 20;

	Enemy.width = Enemy.height = 150;
	Enemy.x = Enemy.y = 300;

	//create a font
	font = MakeFont("Arial", 24);
	
	HRESULT BGresult = d3ddev->CreateOffscreenPlainSurface(
		BUFFERW,					//width of the surface
		BUFFERH,					//height of the surface
		D3DFMT_X8B8G8R8,
		D3DPOOL_DEFAULT,
		&background,				//pointer to the surface
		NULL);

	//copy image to upper left corner of the background
	RECT source_rect = { 0,0,4098, 768 };
	RECT dest_ul = { 0,0, 4098, 768 };
	d3ddev->StretchRect(image, &source_rect, background, &dest_ul, D3DTEXF_NONE);
	//copy image to right corner
	RECT dest_ur = { 4098,0, 4098 * 2, 768 };
	d3ddev->StretchRect(image, &source_rect, background, &dest_ur, D3DTEXF_NONE);
	//copy image to lower left corner
	RECT dest_ll = { 0, 768, 4098, 768 * 2 };
	d3ddev->StretchRect(image, &source_rect, background, &dest_ll, D3DTEXF_NONE);
	//copy image to lower right corner
	RECT dest_lr = { 4098, 768, 4098 * 2, 768 * 2 };
	d3ddev->StretchRect(image, &source_rect, background, &dest_lr, D3DTEXF_NONE);

	//pointer to backbuffer
	d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);

	input1.GetData(WM_INPUT);
	input1.IsLeftMouseButtonDown();
	input1.RegisterDevices();

	image->Release();
	return true;
}


void Game_Run(HWND window)
{	
	FMOD_RESULT			result;
	//make sure the Direct3D device is valid
	if (!d3ddev)return;

	//update the input devices
	DirectInput_Update();

	//clear the scene
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 100), 1.0f, 0);

	
	//parameters for the characters position while in different states
	if (pDirection == LEFT)
	{
		MainChar.scalingX = -1.0f;
		MainChar.x = SCREENW/2+25;
	}
	if (pDirection == RIGHT)
	{
		MainChar.scalingX = 1;
		MainChar.x = SCREENW/2 -25;
	}

	//starting the functions in Game_run

	State_Animate();
	Main.Gravity();
	Main.Platform_Collisions(MainChar, Platforms);
	Main.Jump(MainChar, 64, 73, 73);
	Main.Jump(MainChar, 64, 73, 73);

	//player default state
	playerS = STANDING;
	//move the "MainChar" Left and Right with A and D
	if (input1.IsAlphabeticKeyDown(VKey_A - 0x41) == 1)
	{
		pDirection = LEFT;
		if (CharWait != 1)
		{
			scrollx -= 0.2;
			Platforms[0].x += 0.2;
			Enemy.x += 0.2;
			Platforms[1].x += 0.2;
		}
		if (MainChar.x < 0) MainChar.x = 0;
		playerS = RUNNING;
	}

	if (input1.IsAlphabeticKeyDown(VKey_D - 0x41) == 1)
	{
		pDirection = RIGHT;
		if (CharWait != 1)
		{
			scrollx += 0.2;
			Platforms[0].x -= 0.2;
			Enemy.x -= 0.2;
			Platforms[1].x -= 0.2;
		}
		playerS = RUNNING;
	}

	//Jumping with W
	if (input1.IsAlphabeticKeyDown(VKey_W - 0x41) == 1)
	{
		playerS = JUMPING;
	}

	// Dashing with E
	if (input1.IsAlphabeticKeyDown(VKey_E - 0x41) == 1 && CharWait == 0)
	{
		playerS = DASHING;
		scrollx += 30*MainChar.scalingX;
		Platforms[1].x -= 30*MainChar.scalingX;
	}

	//attacking using the left mouse button
	if (input1.IsLeftMouseButtonDown() == 1 && CharWait == 0)
	{
		playerS = LIGHT_ATK;
	}

	//reseting the scroll buffer

	if (scrolly < 0)
		scrolly = BUFFERH - SCREENH;
	if (scrolly > BUFFERH - SCREENH)
		scrolly = 0;
	if (scrollx < 0)
		scrollx = BUFFERW - SCREENW;
	if (scrollx > BUFFERW - SCREENW)
		scrollx = 0;

	//start rendering
	if (d3ddev->BeginScene())
	{
		
		RECT source_rect = { scrollx,scrolly, scrollx + 1024, scrolly + 768 };
		RECT dest_rect = { 0,0,1024,768 };

		
		d3ddev->StretchRect(background, &source_rect, backbuffer, &dest_rect, D3DTEXF_NONE);
		
		

		spriteobj->Begin(D3DXSPRITE_ALPHABLEND);
		
		Sprite_Transform_Draw(imgPlat[1], Platforms[1].x, Platforms[1].y, Platforms[1].width, Platforms[1].height, Platforms[1].frame, Platforms[1].columns, Platforms[1].rotation, Platforms[1].scalingX, Platforms[1].scalingY, Platforms[1].color);
		
		//stop drawing
		spriteobj->End();

		spriteobj->Begin(D3DXSPRITE_ALPHABLEND);

		Sprite_Transform_Draw(imgEnemy, Enemy.x, Enemy.y, Enemy.width, Enemy.height, Enemy.frame, Enemy.columns, Enemy.rotation, Enemy.scalingX, Enemy.scalingY, Enemy.color);

		//stop drawing
		spriteobj->End();

		spriteobj->Begin(D3DXSPRITE_ALPHABLEND);

		Sprite_Transform_Draw(imgPlat[0], Platforms[0].x, Platforms[0].y, Platforms[0].width, Platforms[0].height, Platforms[0].frame, Platforms[0].columns, Platforms[0].rotation, Platforms[0].scalingX, Platforms[0].scalingY, Platforms[0].color);

		//stop drawing
		spriteobj->End();

		//start drawing
		spriteobj->Begin(D3DXSPRITE_ALPHABLEND);

		std::ostringstream oss2;
		oss2 << "Vel = " << Main.Grav_Velc << "jumping " << Main.jumping << "collision" << Collision(MainChar, Platforms[0]);
		FontPrint(font, 100, 100, oss2.str());

		Sprite_Transform_Draw(imgMain, MainChar.x, MainChar.y, MainChar.width, MainChar.height, MainChar.frame, MainChar.columns, MainChar.rotation, MainChar.scalingX, MainChar.scalingY, MainChar.color);

		/*std::ostringstream oss;
		oss << "scroll = " << scrollx << " Main X = " << MainChar.x << " Floor X = " << Platforms[1].x;
		FontPrint(font, 0, 0, oss.str());*/
		//stop drawing
		spriteobj->End();

		//stop rendering
		d3ddev->EndScene();
		d3ddev->Present(NULL, NULL, NULL, NULL);
	}

	//escape key exits (doesn't work as direct input is currently down)
	if (Key_Down(DIK_ESCAPE))
		gameover = true;
	
	//controller Backbutton also exits (doesn't work as direct input is currently down)
	if (controllers[0].wButtons&XINPUT_GAMEPAD_BACK)
		gameover = true;
}

void Game_End()
{
	if (imgMain)imgMain->Release();
	if (imgFloor)imgFloor->Release();
	if (imgEnemy)imgEnemy->Release();
	if (imgPlat[0])imgPlat[0]->Release();
	if (imgPlat[1])imgPlat[1]->Release();
	if (background)background->Release();
	
	DirectInput_Shutdown();
	Direct3D_Shutdown();
}