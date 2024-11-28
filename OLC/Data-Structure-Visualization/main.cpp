#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <vector>
using namespace std;

class Example : public olc::PixelGameEngine
{
public:
	Example(){	
		sAppName = "Example";
	}

private:
	struct buttons {
		olc::vi2d pos;
		olc::vi2d size = { 400,100 };
	};
	struct listnodes{
		olc::vi2d pos = {160,160};
		listnodes* next = NULL;
		listnodes* prep = NULL;
		string elm="";
	};	
	struct treenodes {
		olc::vi2d pos = { 160,160 };
		treenodes* left = NULL;
		treenodes* right = NULL;
		treenodes* parent = NULL;
		string elm = "";
	};
	struct vertices {
		olc::vi2d pos = { 160,160 };
		string elm = "";
	};

	int cx = 0;
	int cy = 0;

public:
	buttons* linklist = new buttons;
	buttons* bintree = new buttons;
	buttons* graph = new buttons;

	bool menu = true;
	bool ll = false;
	bool texting = false;
	bool btr = false;
	bool gr = false;
	bool choosing = false;
	bool wrong1 = false;
    bool changed = false;
	int listlength = 0;
	int llw = 160;
	int llh = 80;
	int trs = 25;
	int vs = 25;

	listnodes* listhead = NULL;
	listnodes* curlist = NULL;
	treenodes* root = NULL;
	treenodes* curnode = NULL;
	vector<vertices> verts;
	int vertexist[100] = { 0 };
	int h, n = -1;
	int cur = -1;
	int edges[100][100] = { 0 };

	bool OnUserCreate() override
	{	//初始化按钮：
		linklist->pos = olc::vi2d(600, 100);
		bintree->pos = olc::vi2d(600, 400);
		graph->pos = olc::vi2d(600, 700);

		//画：
		Clear(olc::WHITE);
		FillRect(linklist->pos, linklist->size, olc::RED);
		DrawRect(linklist->pos, linklist->size, olc::BLACK);
		DrawString(linklist->pos + olc::vi2d(70,35), "LINKLIST", olc::BLACK, 4);
		FillRect(bintree->pos, bintree->size, olc::RED);
		DrawRect(bintree->pos, bintree->size, olc::BLACK);
		DrawString(bintree->pos + olc::vi2d(40, 35), "BINARYTREE", olc::BLACK, 4);
		FillRect(graph->pos, graph->size, olc::RED);
		DrawRect(graph->pos, graph->size, olc::BLACK);
		DrawString(graph->pos + olc::vi2d(120, 35), "GRAPH", olc::BLACK, 4);

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{	//各个界面
		changed = false;
		if (menu) {
			if(GetMouse(0).bPressed&&GetMouseX()>=600&& GetMouseX() <= 600+linklist->size.x) {
				int cy = GetMouseY();
				if (cy >= linklist->pos.y && cy <= linklist->pos.y + linklist->size.y) {
					ll = true;
					menu = false;
					changed = true;
				}
				if (cy >= bintree->pos.y && cy <= bintree->pos.y + bintree->size.y) {
					btr = true;
					menu = false;
					changed = true;
				}
				if (cy >= graph->pos.y && cy <= graph->pos.y + graph->size.y) {
					gr = true;
					menu = false;
					changed = true;
				}
			}

			if (GetKey(olc::ESCAPE).bPressed){
				return false;
			}
		}

		if (ll) {
			if (GetMouse(0).bPressed) {
				cx = GetMouseX();
				cy = GetMouseY();
				listnodes* tmp = listhead;
				while (tmp != NULL) {
					if (cx >= tmp->pos.x && cx <= tmp->pos.x + llw && cy >= tmp->pos.y && cy <= tmp->pos.y + llh) {
						curlist = tmp;
						changed = true;
						break;
					}
					tmp = tmp->next;
				}
			}

			if (GetMouse(0).bHeld) {
				cx = GetMouseX();
				cy = GetMouseY();
				if (curlist != NULL&&cx >= curlist->pos.x && cx <= curlist->pos.x + llw && cy >= curlist->pos.y && cy <= curlist->pos.y + llh ) {
					curlist->pos = { cx-llw/2,cy-llh/2 };
					changed = true;
				}
			}

			if (GetKey(olc::ESCAPE).bPressed) {
				menu = true;
				ll = false;
				changed = true;
				listlength = 0;
			}

			if (GetKey(olc::I).bPressed) {
				changed = true;
				if (listlength) {
					listlength += 1;
					listnodes* tmp = new listnodes;
					tmp->next = curlist->next;
					tmp->prep = curlist;
					if(curlist->next!=NULL) curlist->next->prep = tmp;
					curlist->next =tmp;
				}
				else {
					listlength += 1;
					listnodes* tmp = new listnodes;
					listhead = tmp;
					curlist = tmp;
				}
				
			}

			if (GetKey(olc::D).bPressed) {
				changed = true;
				if (listlength>1) {
					listlength -= 1;
					if (curlist->prep == NULL) listhead = curlist->next;
					if (curlist->next != NULL) curlist->next->prep = curlist->prep;
					if (curlist->prep != NULL) curlist->prep->next = curlist->next;
					if (curlist->next != NULL) curlist = curlist->next;
					else curlist = curlist -> prep;
				}
				else {
					listlength =0;
					listhead = NULL;
					curlist = NULL;
				}
			}

			if (texting) {
				cin >> curlist->elm;
				texting = false;
				changed = true;				
			}

			if (GetKey(olc::C).bPressed&&texting==false&&listlength!=0) {
				texting = true;
				changed = true;
			}

			if (GetKey(olc::A).bPressed) {
				changed = true;
				int h = (900-llh*(900 / llh - 3))/ (900 / llh - 2); int w = (1600-llw*(1600 / llw - 3))/ (1600 / llw - 2);
				olc::vi2d cpos = { w,h };
				listnodes* tmp = new listnodes;
				tmp = listhead;
				while (tmp != NULL) {
					tmp->pos = cpos;
					if (cpos.x + w + 2 * llw < 1600) {
						cpos.x += (w + llw);
					}
					else {
						cpos.x = w;
						cpos.y += h + llh;
					}
					tmp = tmp->next;
				}
			}

		}

		if (btr) {
			if (GetMouse(0).bPressed) {
				cx = GetMouseX();
				cy = GetMouseY();
				if (root != NULL) {
					treenodes* tmp = root;
					queue<treenodes*> q;
					q.push(tmp);
					while (!q.empty()) {
						tmp = q.front();
						q.pop();
						if (tmp->left != NULL) q.push(tmp->left);
						if (tmp->right != NULL) q.push(tmp->right);
						if ((cx - tmp->pos.x) * (cx - tmp->pos.x) + (cy - tmp->pos.y) * (cy - tmp->pos.y) <= trs * trs) {
							changed = true;
							curnode = tmp;
							break;
						}
					}
				}
			}

			if (GetMouse(0).bHeld) {
				cx = GetMouseX();
				cy = GetMouseY();
				if (curnode != NULL && (cx - curnode->pos.x) * (cx - curnode->pos.x) + (cy - curnode->pos.y) * (cy - curnode->pos.y) <= trs*trs) {
					curnode->pos = { cx,cy };
					changed = true;
				}
			}

			if (GetKey(olc::ESCAPE).bPressed) {
				menu = true;
				btr = false;
				changed = true;
				wrong1 = false;
				texting = false;
				choosing = false;
				root = NULL;
				curnode = NULL;
			}

			if (GetKey(olc::I).bPressed&&choosing==false) {
				if (root == NULL) {
					root = new treenodes;
					curnode = root;
					changed = true;
				}
				else{
					choosing = true;
					changed = true;
				}
			}

			if (choosing) {
				changed = true;
				if (GetKey(olc::L).bPressed) {
					if (curnode->left == NULL) {
						treenodes* tmp = new treenodes;
						curnode->left = tmp;
						tmp->parent = curnode;
						choosing = false;
					}
					else {
						wrong1 = true;
						choosing = false;
					}
				}
				if (GetKey(olc::R).bPressed) {
					if (curnode->right == NULL) {
						treenodes* tmp = new treenodes;
						curnode->right = tmp;
						tmp->parent = curnode;
						choosing = false;
					}
					else {
						wrong1 = true;
						choosing = false;
					}
				}
			}

			if (GetKey(olc::D).bPressed) {
				changed = true;
				if (curnode != NULL && curnode->parent == NULL && root != NULL && curnode->right == NULL && curnode->left == NULL) {
					delete curnode;
					root = NULL;
					curnode = NULL;
				}
				else {
					if (root != NULL && curnode->right == NULL && curnode->left == NULL) {
						treenodes* tmp = curnode->parent;
						if (tmp->left == curnode) {
							tmp->left = NULL;
						}
						else {
							tmp->right = NULL;
						}
						delete curnode;
						curnode = tmp;
					}
					else {
						wrong1 = true;
					}
				}
				
			}

			if (GetKey(olc::A).bPressed) {
				if (root != NULL) {
					changed = true;
					root->pos = olc::vi2d(800, 30);
					treenodes* tmp = root;
					int x;
					queue<treenodes*> q;
					queue<int> level;
					q.push(tmp);
					level.push(4);
					while (!q.empty()) {
						tmp = q.front();
						q.pop();
						x = level.front();
						level.pop();
						if (tmp->left != NULL) {
							q.push(tmp->left);
							level.push(x * 2);
							tmp->left->pos = olc::vi2d(tmp->pos.x - 1600 / x, tmp->pos.y + 80);
						}
						if (tmp->right != NULL) {
							q.push(tmp->right);
							level.push(x * 2);
							tmp->right->pos = olc::vi2d(tmp->pos.x + 1600 / x, tmp->pos.y + 80);
						}
					}
				}
			}

			if (texting) {
				cin >> curnode->elm;
				texting = false;
				changed = true;
			}

			if (GetKey(olc::C).bPressed && texting == false) {
				texting = true;
				changed = true;
			}
		}
		
		if (gr) {
			if (GetMouse(0).bPressed) {
				cx = GetMouseX();
				cy = GetMouseY();
				for (int i = 0; i < verts.size(); ++i){
					if ((cx - verts[i].pos.x) * (cx - verts[i].pos.x) + (cy - verts[i].pos.y) * (cy - verts[i].pos.y) <= vs * vs) {
						changed = true;
						cur = i;
						break;
					}
				}
				if (choosing&&changed) {
					n = cur;
					edges[h][n] = 1;
					choosing = false;
					h = n = -1;
					
				}
			}

			if (GetMouse(1).bPressed&&choosing) {
				cx = GetMouseX();
				cy = GetMouseY();
				for (int i = 0; i < verts.size(); ++i) {
					if ((cx - verts[i].pos.x) * (cx - verts[i].pos.x) + (cy - verts[i].pos.y) * (cy - verts[i].pos.y) <= vs * vs) {
						changed = true;
						
						cur = i;
						break;
					}
				}
				if (changed) {
					n = cur;
					edges[h][n] = 0;
					choosing = false;
					h = n = -1;

				}
			}

			if (GetMouse(0).bHeld) {
				cx = GetMouseX();
				cy = GetMouseY();
				if (cur!=-1 && (cx - verts[cur].pos.x) * (cx - verts[cur].pos.x) + (cy - verts[cur].pos.y) * (cy - verts[cur].pos.y) <= vs * vs) {
					verts[cur].pos = { cx,cy };
					changed = true;
				}
			}

			if (GetKey(olc::ESCAPE).bPressed) {
				menu = true;
				gr = false;
				texting = false;
				choosing = false;
				changed = true;
				for (int i = 0; i < verts.size(); ++i) {
					for (int j = 0; j < verts.size(); ++j) {
						edges[i][j] = 0;
					}
				}
				verts.clear();		
				cur = -1;
			}

			if (GetKey(olc::I).bPressed) {
				changed = true;
				vertices* tmp = new vertices;
				verts.push_back(*tmp);
				vertexist[verts.size() - 1] = 1;

			}

			if (GetKey(olc::D).bPressed) {
				if (cur!=-1) {
					changed = true;
					vertexist[cur] = 0;
					cur = -1;
				}
				
			}
			
			if (texting) {
				cin >> verts[cur].elm;
				texting = false;
				changed = true;
			}

			if (GetKey(olc::C).bPressed && texting == false && cur!=-1) {
				texting = true;
				changed = true;
			}

			if (GetKey(olc::E).bPressed && choosing == false && cur!=-1) {
				changed = true;
				h = cur;
				choosing = true;
				
			}

			if (GetKey(olc::A).bPressed) {
				changed = true;
				int h = 50; int w = 50;
				olc::vi2d cpos = { w+vs,h+vs };
				
				for(int i=0;i<verts.size();++i) {
					if (!vertexist[i]) continue;
					verts[i].pos = cpos;
					if (cpos.x + w + vs*3  < 1600) {
						cpos.x += (w + vs*2);
					}
					else {
						cpos.x = w+vs;
						cpos.y += h + vs*2;
					}
					
				}
			}

		}

		//绘制
		SetPixelMode(olc::Pixel::ALPHA);
		if (changed) {
			if (menu) {
				Clear(olc::WHITE);
				FillRect(linklist->pos, linklist->size, olc::RED);
				DrawRect(linklist->pos, linklist->size, olc::BLACK);
				DrawString(linklist->pos + olc::vi2d(70, 35), "LINKLIST", olc::BLACK, 4);
				FillRect(bintree->pos, bintree->size, olc::RED);
				DrawRect(bintree->pos, bintree->size, olc::BLACK);
				DrawString(bintree->pos + olc::vi2d(40, 35), "BINARYTREE", olc::BLACK, 4);
				FillRect(graph->pos, graph->size, olc::RED);
				DrawRect(graph->pos, graph->size, olc::BLACK);
				DrawString(graph->pos + olc::vi2d(120, 35), "GRAPH", olc::BLACK, 4);
			}

			if (ll) {
				Clear(olc::WHITE);	
				listnodes* tmp = listhead;
				if (listlength == 1) {
					FillRect(tmp->pos, olc::vi2d(llw, llh), olc::Pixel(255,235,205));
					DrawRect(tmp->pos, olc::vi2d(llw / 2, llh), olc::BLACK);
					DrawRect(tmp->pos + olc::vi2d(llw / 2, 0), olc::vi2d(llw / 2, llh), olc::BLACK);					
					DrawString(tmp->pos + olc::vi2d(llw / 15, llh / 5),tmp->elm , olc::BLACK, 3);
				}
				if (listlength > 1) {
					while (tmp->next != NULL) {
						FillRect(tmp->pos, olc::vi2d(llw, llh), olc::Pixel(255, 235, 205));
						DrawRect(tmp->pos, olc::vi2d(llw / 2, llh), olc::BLACK);
						DrawRect(tmp->pos + olc::vi2d(llw / 2, 0), olc::vi2d(llw / 2, llh), olc::BLACK);		
						DrawString(tmp->pos + olc::vi2d(llw / 15, llh / 5), tmp->elm, olc::BLACK, 3);
						olc::vf2d a = tmp->pos + olc::vi2d(llw * 3 / 4, llh / 2);
						olc::vf2d b = tmp->next->pos + olc::vi2d(0, llh / 2);
						DrawLine(a, b,olc::BLACK);
						olc::vf2d c = b - a;
						FillTriangle(b, b - c .norm()*20 + (c.perp()).norm() * 10, b - c.norm() * 20 - (c.perp()).norm() * 10, olc::BLACK);
						tmp = tmp->next;
					}
					FillRect(tmp->pos, olc::vi2d(llw, llh), olc::Pixel(255, 235, 205));
					DrawRect(tmp->pos, olc::vi2d(llw / 2, llh), olc::BLACK);
					DrawRect(tmp->pos + olc::vi2d(llw / 2, 0), olc::vi2d(llw / 2, llh), olc::BLACK);
					DrawString(tmp->pos + olc::vi2d(llw / 15, llh / 5), tmp->elm, olc::BLACK, 3);
				}
				if (listlength > 0) DrawRect(curlist->pos, olc::vi2d(llw, llh), olc::Pixel(255, 0, 0));		
				if (texting) {
					DrawString(olc::vi2d(50, 800), "PRESS ENTER", olc::BLACK,4);
				}
				else {
					DrawString(olc::vi2d(50, 800), "PRESS I TO INPUT,PRESS D TO DELETE\nPRESS C TO CHANGE THE TEXT,PRESS A TO ARRANGE", olc::BLACK, 4);
				}
			}

			if (btr) {
				Clear(olc::WHITE);
				if (root!=NULL) {
					treenodes* tmp = root;
					queue<treenodes*> q;
					q.push(tmp);
					while (!q.empty()) {
						tmp = q.front();
						q.pop();
						if (tmp->left != NULL) {
							olc::vi2d l = tmp->left->pos;
							q.push(tmp->left);
							DrawLine(tmp->pos, l, olc::BLACK);
						}
						if (tmp->right != NULL) {
							olc::vi2d r = tmp->right->pos;
							q.push(tmp->right);
							DrawLine(tmp->pos, r, olc::BLACK);
						}
						FillCircle(tmp->pos, trs, olc::Pixel(255, 235, 205));
						DrawCircle(tmp->pos, trs, olc::BLACK);
						DrawString(tmp->pos-olc::vi2d(10,10), tmp->elm, olc::BLACK, 2);
					}
				}
				if (curnode!=NULL) DrawCircle(curnode->pos, trs, olc::RED);
				if (choosing && !wrong1) {
					DrawString(olc::vi2d(50, 800), "PRESS L OR R TO CHOOSE", olc::BLACK, 4);
				}
				if (texting && !wrong1) {
					DrawString(olc::vi2d(50, 800), "PRESS ENTER", olc::BLACK, 4);
				}
				if(!choosing&&!texting && !wrong1){
					DrawString(olc::vi2d(50, 800), "PRESS I TO INPUT,PRESS D TO DELETE\nPRESS C TO CHANGE THE TEXT,PRESS A TO ARRANGE", olc::BLACK, 4);
				}
				if (wrong1) {
					DrawString(olc::vi2d(50, 800), "INVALID OPERATION", olc::BLACK, 4);
					wrong1 = false;
				}
			}

			if (gr) {
				Clear(olc::WHITE);
				for (int i = 0; i < verts.size(); ++i) {
					for (int j = 0; j < verts.size(); ++j) {
						if (edges[i][j]&&vertexist[i]&&vertexist[j]) {					
							olc::vf2d c = (verts[j].pos - verts[i].pos);
							c = c.norm();
							olc::vf2d b = verts[j].pos - c * vs;
							FillTriangle(b, b - c * 20 + c.perp() * 10, b - c * 20 - c.perp() * 10, olc::BLACK);
							DrawLine(verts[i].pos, verts[j].pos, olc::BLACK);
						}
					}	
				}
				for (int i = 0; i < verts.size(); ++i) {
					if (vertexist[i]) {
						FillCircle(verts[i].pos, trs, olc::Pixel(255, 235, 205));
						DrawCircle(verts[i].pos, trs, olc::BLACK);
						DrawString(verts[i].pos-olc::vi2d(10,10), verts[i].elm, olc::BLACK, 2);
					}		
				}
				if (cur!=-1) DrawCircle(verts[cur].pos, trs, olc::RED);
				if (choosing) {
					DrawString(olc::vi2d(50, 800), "CLICK TO CHOOSE THE VERT\nLEFT TO CREATE,RIGHT TO DELETE", olc::BLACK, 4);
				}
				if (texting) {
					DrawString(olc::vi2d(50, 800), "PRESS ENTER TO CONFIRM", olc::BLACK, 4);
				}
				if (!choosing && !texting) {
					DrawString(olc::vi2d(50, 800), "PRESS I TO INPUT,PRESS D TO DELETE\nPRESS C TO CHANGE THE TEXT,PRESS A TO ARRANGE", olc::BLACK, 4);
				}			
			}
		}

		return true;
	}
};

int main()
{
	Example demo;
	if (demo.Construct(1600, 900, 1, 1))
		demo.Start();
	return 0;
}