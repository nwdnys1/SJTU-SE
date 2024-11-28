#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <iostream>
#include <fstream>
#include <string>
#include <queue>
using namespace std;

class Example : public olc::PixelGameEngine
{
public:
	Example()
	{
		sAppName = "Example";
	}

private:
	struct units {
		char type;//k为骑士 f为步兵 n为空 a为弓兵
		int side;//1为a 0为b 2为空
		int ap;//体力
	};
	struct block {
		units unit = {'n',2,0};
		char field = 'p';
	};
	olc::vi2d vBlockSize = { 80,80 };
	std::unique_ptr<block[]> blocks;
	std::unique_ptr<olc::Sprite> ft1;
	std::unique_ptr<olc::Sprite> ft2;
	std::unique_ptr<olc::Sprite> kn1;
	std::unique_ptr<olc::Sprite> kn2;
	std::unique_ptr<olc::Sprite> ac1;
	std::unique_ptr<olc::Sprite> ac2;
	std::unique_ptr<olc::Sprite> mg1;
	std::unique_ptr<olc::Sprite> mg2;
	std::unique_ptr<olc::Sprite> pl;
	std::unique_ptr<olc::Sprite> fr;
	std::unique_ptr<olc::Sprite> br;
	std::unique_ptr<olc::Sprite> wt;
	std::unique_ptr<olc::Sprite> mt;
	std::unique_ptr<olc::Sprite> ab;
	std::unique_ptr<float[]> burns;
	int cx=0;//当前选中的格子
	int cy=0;
	olc::vf2d fbPos = { 0.0f, 0.0f };
	olc::vf2d fbDir = { 0.0f, 0.0f };
	float fbSpeed = 200.0f;



public:
	int height;//几格
	int width;
	bool side = 1;
	bool mgattack = false;
	bool fireball = false;
	bool fbstart = false;
	bool earthquake = false;
	queue<olc::vi2d> que;

	bool OnUserCreate() override
	{	
		blocks = std::make_unique<block[]>(width * height);
		burns = std::make_unique<float[]>(width * height);
		
		//输入地图：
		cout << "enter the field:(x,y,field,unit,side),press q if finished" << endl;
		int tmp;
		while (cin >> tmp) {
			int x,y,s; char f,u;
			if (tmp == 'q') break;
			else {
				x = tmp;
				cin >> y >> f >> u>>s;
				blocks[y * width + x].field = f;
				blocks[y * width + x].unit.type = u;
				blocks[y * width + x].unit.side = s;
			}
		}
	
		//初始化体力：
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				if (blocks[y * width + x].unit.type == 'k') {
					blocks[y * width + x].unit.ap = 5;
				}
				if (blocks[y * width + x].unit.type == 'f') {
					blocks[y * width + x].unit.ap = 4;
				}
				if (blocks[y * width + x].unit.type == 'a') {
					blocks[y * width + x].unit.ap = 3;
				}
				if (blocks[y * width + x].unit.type == 'm') {
					blocks[y * width + x].unit.ap = 2;
				}
			}
		}

		//初始化精灵：
		ft1 = std::make_unique<olc::Sprite>("./../../ft1.png");
		ft2 = std::make_unique<olc::Sprite>("../../ft2.png");
		kn1 = std::make_unique<olc::Sprite>("../../kn1.png");
		kn2 = std::make_unique<olc::Sprite>("../../kn2.png");
		ac1 = std::make_unique<olc::Sprite>("../../ac1.png");
		ac2 = std::make_unique<olc::Sprite>("../../ac2.png");
		mg1 = std::make_unique<olc::Sprite>("../../mg1.png");
		mg2 = std::make_unique<olc::Sprite>("../../mg2.png");
		pl = std::make_unique<olc::Sprite>("../../pl.jpg");
		ab = std::make_unique<olc::Sprite>("../../ab.jpg");
		mt = std::make_unique<olc::Sprite>("../../mt.jpg");
		fr = std::make_unique<olc::Sprite>("../../fr.png");
		wt = std::make_unique<olc::Sprite>("../../wt.jpg");
		br = std::make_unique<olc::Sprite>("../../br.png");

		//画：
		Clear(olc::WHITE);
		SetPixelMode(olc::Pixel::MASK); // Dont draw pixels which have any transparency

		//下方字幕：
		if (side) DrawString(olc::vi2d(40, 80 * height + 10), "A's TURN", olc::BLACK, 4);
		else DrawString(olc::vi2d(40, 80 * height + 10), "B's TURN", olc::BLACK, 4);
		string career;
		switch (blocks[cy * width + cx].unit.type) {
		case('a'):	career = "CAREER:Archer  "; break;
		case('m'):	career = "CAREER:Mage  "; break;
		case('f'):	career = "CAREER:Footman  "; break;
		case('k'):	career = "CAREER:Knight  "; break;
		}
		string str = career + "AP:" + to_string(blocks[cy * width + cx].unit.ap);
		DrawString(olc::vi2d(40, 80 * height + 50), str, olc::BLACK, 4);

		if (!mgattack && !fireball && !fbstart && !earthquake) DrawString(olc::vi2d(40, 80 * height + 90), "RIGHT CLICK TO ATTACK\nCLICK T TO CHANGE TURNS", olc::BLACK, 4);
		if (mgattack) DrawString(olc::vi2d(40, 80 * height + 90), "1:FIREBALL 2:EARTHQUAKE", olc::BLACK, 4);
		if (fireball) DrawString(olc::vi2d(40, 80 * height + 90), "CHOOSE THE DIRECTION", olc::BLACK, 4);
		if (earthquake) DrawString(olc::vi2d(40, 80 * height + 90), "RIGHT CLICK TO CHOOSE WHERE", olc::BLACK, 4);

		//每一格：
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				switch (blocks[y * width + x].field)
				{
				case 'p': // Do nothing
					DrawPartialSprite(olc::vi2d(x, y) * vBlockSize, pl.get(), olc::vi2d(0, 0), olc::vi2d(80, 80), 1);
					break;
				case 'w': // Do nothing
					DrawPartialSprite(olc::vi2d(x, y) * vBlockSize, wt.get(), olc::vi2d(0, 0), olc::vi2d(80, 80), 1);
					blocks[y * width + x].unit.type = 'n'; blocks[y * width + x].unit.ap = 0; blocks[y * width + x].unit.side = 2;
					break;
				case 'f': // Do nothing
					DrawPartialSprite(olc::vi2d(x, y) * vBlockSize, fr.get(), olc::vi2d(0, 0), olc::vi2d(80, 80), 1);
					break;
				case 'm': // Do nothing
					DrawPartialSprite(olc::vi2d(x, y) * vBlockSize, mt.get(), olc::vi2d(0, 0), olc::vi2d(80, 80), 1);
					blocks[y * width + x].unit.type = 'n'; blocks[y * width + x].unit.ap = 0; blocks[y * width + x].unit.side = 2;
					break;
				case 'b': // Do nothing
					DrawPartialSprite(olc::vi2d(x, y) * vBlockSize, br.get(), olc::vi2d(0, 0), olc::vi2d(80, 80), 1);
					blocks[y * width + x].unit.type = 'n'; blocks[y * width + x].unit.ap = 0; blocks[y * width + x].unit.side = 2;
					if (burns[y * width + x]-- == 0) blocks[y * width + x].field = 'p';
					break;
				case 'a': // Do nothing
					if (blocks[(y + 1) * width + (x + 1)].field == 'w' && x + 1 < width && y + 1 < height) blocks[y * width + x].field = 'w';
					if (blocks[(y + 1) * width + (x - 1)].field == 'w' && x - 1 > 0 && y + 1 < height) blocks[y * width + x].field = 'w';
					if (blocks[(y - 1) * width + (x + 1)].field == 'w' && x + 1 < width && y - 1 > 0) blocks[y * width + x].field = 'w';
					if (blocks[(y - 1) * width + (x - 1)].field == 'w' && x - 1 > 0 && y - 1 > 0) blocks[y * width + x].field = 'w';
					if (blocks[(y + 1) * width + (x)].field == 'w' && y + 1 < height) blocks[y * width + x].field = 'w';
					if (blocks[(y - 1) * width + (x)].field == 'w' && y - 1 > 0) blocks[y * width + x].field = 'w';
					if (blocks[(y)*width + (x + 1)].field == 'w' && x + 1 < width) blocks[y * width + x].field = 'w';
					if (blocks[(y)*width + (x - 1)].field == 'w' && x - 1 > 0) blocks[y * width + x].field = 'w';
					DrawPartialSprite(olc::vi2d(x, y) * vBlockSize, ab.get(), olc::vi2d(0, 0), olc::vi2d(80, 80), 1);
					blocks[y * width + x].unit.type = 'n'; blocks[y * width + x].unit.ap = 0; blocks[y * width + x].unit.side = 2;
					break;
				}
			}
		}

		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				switch (blocks[y * width + x].unit.type)
				{
				case 'n': // Do nothing
					break;
				case 'k': // Do nothing
					if (blocks[y * width + x].unit.side) {
						DrawPartialSprite(olc::vi2d(x, y) * vBlockSize, kn1.get(), olc::vi2d(0, 0), olc::vi2d(80, 80), 1);
					}
					else {
						DrawPartialSprite(olc::vi2d(x, y) * vBlockSize, kn2.get(), olc::vi2d(0, 0), olc::vi2d(80, 80), 1);
					}
					break;
				case 'f': // Do nothing
					if (blocks[y * width + x].unit.side) {
						DrawPartialSprite(olc::vi2d(x, y) * vBlockSize, ft1.get(), olc::vi2d(0, 0), olc::vi2d(80, 80), 1);
					}
					else {
						DrawPartialSprite(olc::vi2d(x, y) * vBlockSize, ft2.get(), olc::vi2d(0, 0), olc::vi2d(80, 80), 1);
					}
					break;
				case 'a': // Do nothing
					if (blocks[y * width + x].unit.side) {
						DrawPartialSprite(olc::vi2d(x, y) * vBlockSize, ac1.get(), olc::vi2d(0, 0), olc::vi2d(80, 80), 1);
					}
					else {
						DrawPartialSprite(olc::vi2d(x, y) * vBlockSize, ac2.get(), olc::vi2d(0, 0), olc::vi2d(80, 80), 1);
					}
					break;
				case 'm': // Do nothing
					if (blocks[y * width + x].unit.side) {
						DrawPartialSprite(olc::vi2d(x, y) * vBlockSize, mg1.get(), olc::vi2d(0, 0), olc::vi2d(80, 80), 1);
					}
					else {
						DrawPartialSprite(olc::vi2d(x, y) * vBlockSize, mg2.get(), olc::vi2d(0, 0), olc::vi2d(80, 80), 1);
					}
					break;
				}
			}
		}

		DrawRect(olc::vi2d(cx, cy) * vBlockSize, vBlockSize, olc::BLACK);


		return true;
	}

	//移动的函数：
	bool apdown(block* old,block*cur) {
		if (cur->unit.type == 'n') {
					if (cur->field == 'p' && old->unit.ap >= 1) {
						cur->unit.ap = old->unit.ap - 1;
						cur->unit.side = old->unit.side;
						cur->unit.type = old->unit.type;
						old->unit.type = 'n';
						old->unit.ap = 0;
						return true;
					}
					if (cur->field == 'f' && old->unit.ap >= 2) {
						cur->unit.ap = old->unit.ap - 2;
						cur->unit.side = old->unit.side;
						cur->unit.type = old->unit.type;
						old->unit.type = 'n';
						old->unit.ap = 0;
						return true;
					}
		}
		return false;
	}

	//地震函数：
	void eq(block &t) {
		if (t.field == 'p' || t.field == 'f') t.field = 'a';
		if (t.field == 'm') t.field = 'p';
	}


	bool OnUserUpdate(float fElapsedTime) override
	{	//检查获胜：
		bool flag1 = true; bool flag2 = true; bool changed = false;
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				if (blocks[y * width + x].unit.side == 1 && blocks[y * width + x].unit.type != 'n') {
					flag1 = false;
				}
				if (blocks[y * width + x].unit.side == 0 && blocks[y * width + x].unit.type != 'n') {
					flag2 = false;
				}
			}
		}
		if (flag1&&flag2) {
			cout << "Draw!";
			return false;
		}
		if (flag1) {
			cout << "B wins!";
			return false;
		}
		if (flag2) {
			cout << "A wins!";
			return false;
		}

		//更换回合：
		if (GetKey(olc::T).bPressed && !mgattack && !fireball && !fbstart && !earthquake) {
			side = !side; 
			if (side) {
				for (int y = 0; y < height; y++){
					for (int x = 0; x < width; x++) {
						if (blocks[y * width + x].unit.type == 'k') {
							blocks[y * width + x].unit.ap = 5;
						}
						if (blocks[y * width + x].unit.type == 'f') {
							blocks[y * width + x].unit.ap = 4;
						}
						if (blocks[y * width + x].unit.type == 'a') {
							blocks[y * width + x].unit.ap = 3;
						}
						if (blocks[y * width + x].unit.type == 'm') {
							blocks[y * width + x].unit.ap = 2;
						}
					}
				}
			}
		}

		//移动：
		if (blocks[cy * width + cx].unit.side == side && blocks[cy * width + cx].unit.type != 'n' && blocks[cy * width + cx].unit.ap > 0 && !mgattack && !fireball && !fbstart && !earthquake) {
			changed = 1;
			block* old = &blocks[cy * width + cx];
			if (GetKey(olc::LEFT).bPressed && cx > 0) {
				block* cur = &blocks[cy * width + cx - 1];
				if(apdown(old, cur))
				cx -= 1;
			}
			if (GetKey(olc::RIGHT).bPressed && cx < width - 1) {
				block* cur = &blocks[cy * width + cx + 1];
				if (apdown(old, cur))
				cx += 1;
			}
			if (GetKey(olc::UP).bPressed && cy > 0) {
				block* cur = &blocks[(cy - 1) * width + cx];
				if (apdown(old, cur))
				cy -= 1;
			}
			if (GetKey(olc::DOWN).bPressed && cy < height - 1) {
				block* cur = &blocks[(cy + 1) * width + cx];
				if (apdown(old, cur))
				cy += 1;
			}
			
		}

		//攻击：
		if (GetMouse(1).bPressed && !mgattack && !fireball && !fbstart && !earthquake && blocks[cy * width + cx].unit.side == side) {
			changed = true;
			int tx = (GetMouseX()) / 80;int ty = (GetMouseY()) / 80;
			if (!(tx == cx && ty == cy)) {
				//一格攻击：
				if (abs(tx - cx) <= 1 && abs(ty - cy) <= 1 && (blocks[cy * width + cx].unit.type == 'k'|| blocks[cy * width + cx].unit.type == 'f')) {
					if (blocks[ty * width + tx].unit.type != 'n') {
						blocks[ty * width + tx].unit.type = 'n';
					}
				}
				//弓兵：
				if (blocks[cy * width + cx].unit.type == 'a') {
					if (abs(tx - cx) == 2 && abs(ty - cy) == 2&&blocks[(tx+cx)/2+(ty+cy)/2*width].field!='m') {
						if (blocks[ty * width + tx].unit.type != 'n') {
							blocks[ty * width + tx].unit.type = 'n';
						}
					}
					if (abs(tx - cx) == 2 && abs(ty - cy) == 0 && blocks[(tx + cx) / 2 + cy* width].field != 'm') {
						if (blocks[ty * width + tx].unit.type != 'n') {
							blocks[ty * width + tx].unit.type = 'n';
						}
					}
					if (abs(tx - cx) == 0 && abs(ty - cy) == 2 && blocks[ cx+ (ty + cy) / 2 * width].field != 'm') {
						if (blocks[ty * width + tx].unit.type != 'n') {
							blocks[ty * width + tx].unit.type = 'n';
						}
					}
				}
			}
			//法师：
			if (blocks[cy * width + cx].unit.type == 'm') mgattack = true;
		}

		//法师攻击特判：
		if (mgattack) {
			if (GetKey(olc::K1).bPressed) {
				mgattack = false;
				fireball = true;
				fbPos = {float(cx) * 80.0f + 40.0f, float(cy) * 80.0f + 40.0f};
			
			}
			if (GetKey(olc::K2).bPressed) {
				mgattack = false;
				earthquake = true;
			}
		}

		if (fireball) {
			if (GetKey(olc::UP).bPressed) {
				fbDir = { 0,-1 };
				fbPos += 41 * fbDir;
				fireball = false;
				fbstart = true;
			}
			if (GetKey(olc::DOWN).bPressed) {
				fbDir = { 0,1 };
				fbPos += 41 * fbDir;
				fireball = false;
				fbstart = true;
			}
			if (GetKey(olc::LEFT).bPressed) {
				fbDir = { -1,0 };
				fbPos += 41 * fbDir;
				fireball = false;
				fbstart = true;
			}
			if (GetKey(olc::RIGHT).bPressed) {
				fbDir = { 1,0 };
				fbPos += 41 * fbDir;
				fireball = false;
				fbstart = true;
			}
		}
		
		if (earthquake) {
			if (GetMouse(1).bPressed) {
				earthquake = false;
				int tx = (GetMouseX()) / 80; int ty = (GetMouseY()) / 80;
				if ((abs(tx - cx) == 3 && abs(ty - cy) == 0) || (abs(tx - cx) == 0 && abs(ty - cy) == 3)) {
					eq(blocks[ty * width + tx]);
					if (tx + 1 < width) eq(blocks[ty * width + tx + 1]);
					if (tx - 1 >= 0) eq(blocks[ty * width + tx - 1]);
					if (ty + 1 < height) eq(blocks[(ty + 1) * width + tx]);
					if (ty - 1 >=0) eq(blocks[(ty - 1) * width + tx]);							
				}
			}
		}
		

		//鼠标选中：
		if (GetMouse(0).bPressed&& (GetMouseY()) / 80 <= height - 1&&!mgattack&&!fireball&&!fbstart&&!earthquake) {
			cx = (GetMouseX())/80;
			cy = (GetMouseY())/80;
			changed = true;
		}

		if (changed){
			//画：
			Clear(olc::WHITE);
			SetPixelMode(olc::Pixel::MASK); // Dont draw pixels which have any transparency

			//下方字幕：
			if (side) DrawString(olc::vi2d(40, 80 * height + 10), "A's TURN", olc::BLACK, 4);
			else DrawString(olc::vi2d(40, 80 * height + 10), "B's TURN", olc::BLACK, 4);
			string career;
			switch (blocks[cy * width + cx].unit.type) {
			case('a'):	career = "CAREER:Archer  "; break;
			case('m'):	career = "CAREER:Mage  "; break;
			case('f'):	career = "CAREER:Footman  "; break;
			case('k'):	career = "CAREER:Knight  "; break;
			}
			string str = career + "AP:" + to_string(blocks[cy * width + cx].unit.ap);
			DrawString(olc::vi2d(40, 80 * height + 50), str, olc::BLACK, 4);

			if (!mgattack && !fireball && !fbstart && !earthquake) DrawString(olc::vi2d(40, 80 * height + 90), "RIGHT CLICK TO ATTACK\nCLICK T TO CHANGE TURNS", olc::BLACK, 4);
			if (mgattack) DrawString(olc::vi2d(40, 80 * height + 90), "1:FIREBALL 2:EARTHQUAKE", olc::BLACK, 4);
			if (fireball) DrawString(olc::vi2d(40, 80 * height + 90), "CHOOSE THE DIRECTION", olc::BLACK, 4);
			if (earthquake) DrawString(olc::vi2d(40, 80 * height + 90), "RIGHT CLICK TO CHOOSE WHERE", olc::BLACK, 4);

			//每一格：
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					switch (blocks[y * width + x].field)
					{
					case 'p': // Do nothing
						DrawPartialSprite(olc::vi2d(x, y) * vBlockSize, pl.get(), olc::vi2d(0, 0), olc::vi2d(80, 80), 1);
						break;
					case 'w': // Do nothing
						DrawPartialSprite(olc::vi2d(x, y) * vBlockSize, wt.get(), olc::vi2d(0, 0), olc::vi2d(80, 80), 1);
						blocks[y * width + x].unit.type = 'n'; blocks[y * width + x].unit.ap = 0; blocks[y * width + x].unit.side = 2;
						break;
					case 'f': // Do nothing
						DrawPartialSprite(olc::vi2d(x, y) * vBlockSize, fr.get(), olc::vi2d(0, 0), olc::vi2d(80, 80), 1);
						break;
					case 'm': // Do nothing
						DrawPartialSprite(olc::vi2d(x, y) * vBlockSize, mt.get(), olc::vi2d(0, 0), olc::vi2d(80, 80), 1);
						blocks[y * width + x].unit.type = 'n'; blocks[y * width + x].unit.ap = 0; blocks[y * width + x].unit.side = 2;
						break;
					case 'b': // Do nothing
						DrawPartialSprite(olc::vi2d(x, y) * vBlockSize, br.get(), olc::vi2d(0, 0), olc::vi2d(80, 80), 1);
						blocks[y * width + x].unit.type = 'n'; blocks[y * width + x].unit.ap = 0; blocks[y * width + x].unit.side = 2;
						if (burns[y * width + x]-- == 0) blocks[y * width + x].field = 'p';
						break;
					case 'a': // Do nothing
						if (blocks[(y + 1) * width + (x + 1)].field == 'w' && x + 1 < width && y + 1 < height) blocks[y * width + x].field = 'w';
						if (blocks[(y + 1) * width + (x - 1)].field == 'w' && x - 1 > 0 && y + 1 < height) blocks[y * width + x].field = 'w';
						if (blocks[(y - 1) * width + (x + 1)].field == 'w' && x + 1 < width && y - 1 > 0) blocks[y * width + x].field = 'w';
						if (blocks[(y - 1) * width + (x - 1)].field == 'w' && x - 1 > 0 && y - 1 > 0) blocks[y * width + x].field = 'w';
						if (blocks[(y + 1) * width + (x)].field == 'w' && y + 1 < height) blocks[y * width + x].field = 'w';
						if (blocks[(y - 1) * width + (x)].field == 'w' && y - 1 > 0) blocks[y * width + x].field = 'w';
						if (blocks[(y)*width + (x + 1)].field == 'w' && x + 1 < width) blocks[y * width + x].field = 'w';
						if (blocks[(y)*width + (x - 1)].field == 'w' && x - 1 > 0) blocks[y * width + x].field = 'w';
						DrawPartialSprite(olc::vi2d(x, y) * vBlockSize, ab.get(), olc::vi2d(0, 0), olc::vi2d(80, 80), 1);
						blocks[y * width + x].unit.type = 'n'; blocks[y * width + x].unit.ap = 0; blocks[y * width + x].unit.side = 2;
						break;
					}
				}
			}

			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					switch (blocks[y * width + x].unit.type)
					{
					case 'n': // Do nothing
						break;
					case 'k': // Do nothing
						if (blocks[y * width + x].unit.side) {
							DrawPartialSprite(olc::vi2d(x, y) * vBlockSize, kn1.get(), olc::vi2d(0, 0), olc::vi2d(80, 80), 1);
						}
						else {
							DrawPartialSprite(olc::vi2d(x, y) * vBlockSize, kn2.get(), olc::vi2d(0, 0), olc::vi2d(80, 80), 1);
						}
						break;
					case 'f': // Do nothing
						if (blocks[y * width + x].unit.side) {
							DrawPartialSprite(olc::vi2d(x, y) * vBlockSize, ft1.get(), olc::vi2d(0, 0), olc::vi2d(80, 80), 1);
						}
						else {
							DrawPartialSprite(olc::vi2d(x, y) * vBlockSize, ft2.get(), olc::vi2d(0, 0), olc::vi2d(80, 80), 1);
						}
						break;
					case 'a': // Do nothing
						if (blocks[y * width + x].unit.side) {
							DrawPartialSprite(olc::vi2d(x, y) * vBlockSize, ac1.get(), olc::vi2d(0, 0), olc::vi2d(80, 80), 1);
						}
						else {
							DrawPartialSprite(olc::vi2d(x, y) * vBlockSize, ac2.get(), olc::vi2d(0, 0), olc::vi2d(80, 80), 1);
						}
						break;
					case 'm': // Do nothing
						if (blocks[y * width + x].unit.side) {
							DrawPartialSprite(olc::vi2d(x, y) * vBlockSize, mg1.get(), olc::vi2d(0, 0), olc::vi2d(80, 80), 1);
						}
						else {
							DrawPartialSprite(olc::vi2d(x, y) * vBlockSize, mg2.get(), olc::vi2d(0, 0), olc::vi2d(80, 80), 1);
						}
						break;
					}
				}
			}

			DrawRect(olc::vi2d(cx, cy) * vBlockSize, vBlockSize, olc::BLACK);
		}
	
		
		//火球：
		if (fbstart) {
			FillCircle(fbPos, 15, olc::RED);
			fbPos= fbPos + fbDir * fbSpeed * fElapsedTime;
			olc::vi2d testpos = { int(fbPos.x) / 80,int(fbPos.y) / 80 };
			if (fbPos.x<0 || fbPos.x >width*80 || fbPos.y < 0 || fbPos.y >height*80) { fbstart = false; }
			else {
				switch (blocks[testpos.y*width+testpos.x].field){	
					case('m'):
						fbstart = false;
						break;
					case('f'):
						fbstart = false;
						que.push(testpos);
						while (!que.empty()) {
							olc::vi2d tmp = que.front();
							que.pop();
							blocks[(tmp.y) * width + (tmp.x)].field = 'b'; burns[(tmp.y) * width + (tmp.x)] = 100.0f;
							if (blocks[(tmp.y + 1) * width + (tmp.x + 1)].field == 'f') que.push({ tmp.x + 1,tmp.y + 1 });
							if (blocks[(tmp.y + 1) * width + (tmp.x - 1)].field == 'f') que.push({ tmp.x - 1,tmp.y + 1 });
							if (blocks[(tmp.y - 1) * width + (tmp.x + 1)].field == 'f') que.push({ tmp.x + 1,tmp.y - 1 });
							if (blocks[(tmp.y - 1) * width + (tmp.x - 1)].field == 'f') que.push({ tmp.x - 1,tmp.y - 1 });
							if (blocks[(tmp.y - 1) * width + (tmp.x)].field == 'f') que.push({ tmp.x,tmp.y - 1 });
							if (blocks[(tmp.y + 1) * width + (tmp.x)].field == 'f') que.push({ tmp.x,tmp.y + 1 });
							if (blocks[(tmp.y) * width + (tmp.x + 1)].field == 'f') que.push({ tmp.x + 1,tmp.y });
							if (blocks[(tmp.y) * width + (tmp.x - 1)].field == 'f') que.push({ tmp.x - 1,tmp.y });
						}
						break;
				}
				if (blocks[testpos.y * width + testpos.x].unit.type != 'n') {
					blocks[testpos.y * width + testpos.x].unit.type = 'n';
					fbstart = false;
				}
			}
			
		}

		SetPixelMode(olc::Pixel::NORMAL); // Draw all pixels
		return true;
	}
};


int main()
{
	Example demo;
	cout << "输入行数列数：" << endl;
	//int m, n;
	//cin >> m >> n;
	demo.height = 11; demo.width = 11;
	if (demo.Construct(80*demo.width+1,80*demo.height+161, 1, 1))
		demo.Start();

	return 0;
}
