#pragma once
#include "EularOp.h"
#include "Sweep.h"
#include <GL/glut.h>
#include <map>

std::map<float, std::map<float, std::map<float, int>>> m;
std::vector<double*> mem_point;
Solid* product();

// 构建映射和渲染
#define ADD_VERTEX(point)\
{\
	m[point[0]][point[1]][point[2]] = mem_point.size();\
	double *_tmp_p = new double[6]{ point[0],point[1],point[2],color[c_s % color_num][0],color[c_s % color_num][1], color[(c_s++) % color_num][2] };\
	mem_point.push_back(_tmp_p); \
	int _id = mem_point.size()-1;\
}\

//颜色信息
const int color_num = 9;
int c_s = 0;
GLdouble color[color_num][3] = { {0.3, 0.5, 1.0},{0.3, 0.5, 1.0},{0.3, 0.5, 1.0},
								 {0.3, 0.5, 1.0},{0.3, 0.5, 1.0},{0.3, 0.5, 1.0},
								 {0.3, 0.5, 1.0},{0.3, 0.5, 1.0},{0.3, 0.5, 1.0}, };

Solid* product() {

	float point[15][3] = {
		{-2,1.5,0	  },	{ -2,-1.5,0  },		{ 2,-1.5,0   },		{ 2,1.5,0   },
		{ -1.5,0.2,0 },		{ -1.5,-0.8,0  },   { -0.5,-0.8,0  },    { -0.5,0.2,0  },
		{ 0.5,0.2,0  },     { 0.5,-0.8,0  },    {   1.5,-0.8,0 },      {  1.5,0.2,0 },
		{-0.5,0.5,0},     {0.5,0.5,0},      {0, 1.4, 0}
	};
	for (int i = 0; i < 15;i++) {
		ADD_VERTEX(point[i])
	}
	Face* f[5];
	Edge* e;
	Vertex* v[15];
	Solid* s = nullptr;
	Loop* lp_tmp;
	// 构造一个方形
	mvfs(&v[0], &f[0], &s, point[0]);
	mev(&e, &v[1], (s->sface->floop), v[0], point[1]);
	mev(&e, &v[2], (s->sface->floop), v[1], point[2]);
	mev(&e, &v[3], (s->sface->floop), v[2], point[3]);
	mef(&e, &f[1], v[3], v[0], f[0]->floop);

	//构造內环1
	mev(&e, &v[4], f[0]->floop, v[0], point[4]);
	mev(&e, &v[7], f[0]->floop, v[4], point[7]);
	mev(&e, &v[6], f[0]->floop, v[7], point[6]);
	mev(&e, &v[5], f[0]->floop, v[6], point[5]);
	mef(&e, &f[2], v[5], v[4], f[0]->floop);
	kemr(&lp_tmp, v[4], v[0], f[0]->floop);
	//构造内环2
	mev(&e, &v[8], f[0]->floop, v[0], point[8]);
	mev(&e, &v[11], f[0]->floop, v[8], point[11]);
	mev(&e, &v[10], f[0]->floop, v[11], point[10]);
	mev(&e, &v[9], f[0]->floop, v[10], point[9]);
	mef(&e, &f[3], v[9], v[8], f[0]->floop);
	kemr(&lp_tmp, v[8], v[0], f[0]->floop);
	//构造内环3
	mev(&e, &v[12], f[0]->floop, v[0], point[12]);
	mev(&e, &v[14], f[0]->floop, v[12], point[14]);
	mev(&e, &v[13], f[0]->floop, v[14], point[13]);;
	mef(&e, &f[4], v[13], v[12], f[0]->floop);
	kemr(&lp_tmp, v[12], v[0], f[0]->floop);

	std::vector<Vertex*> new_v;
	float direction[3] = {0,0,1.5};
	sweep(s->sface, direction, new_v, f[1]);
	for (int i = 0; i < new_v.size(); i++) {
		float tmp_point[3] = { new_v[i]->x,new_v[i]->y, new_v[i]->z };
		ADD_VERTEX(tmp_point)
	}

	return s;
}
