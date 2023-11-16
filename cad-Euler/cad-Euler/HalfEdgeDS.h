#pragma once

class Solid;
class Face;
class Loop;
class Edge;
class HalfEdge;
class Vertex;

class Solid
{
public:
	Solid* prevs, * nexts;
	Face* sface;
	Edge* sedge;
	Solid() :prevs(nullptr), nexts(nullptr), sface(nullptr), sedge(nullptr) { }
};

class Face
{
public:
	static int num;
	int id;
	Face* prevf, * nextf;
	Solid* fsolid;
	Loop* floop;
	Face() :prevf(nullptr), nextf(nullptr), fsolid(nullptr), floop(nullptr) { id = num++; }

};
int Face::num = 0;

class Loop
{
public:
	Loop* prevl, * nextl;
	Face* lface;
	HalfEdge* ledg;
	bool inner;
	Loop() :prevl(nullptr), nextl(nullptr), lface(nullptr), ledg(nullptr), inner(false) { };
};

class HalfEdge
{
public:
	HalfEdge* prev, * next;
	Loop* wloop;
	Edge* edg;
	Vertex* startv;
	HalfEdge() :prev(nullptr), next(nullptr), wloop(nullptr), edg(nullptr), startv(nullptr) { }
};

class Edge
{
public:
	HalfEdge* he1, * he2;
	Edge() :he1(nullptr), he2(nullptr) { }
};

class Vertex
{
public:
	float x, y, z;
	Vertex(float point[3]) :x(point[0]), y(point[1]), z(point[2]) { }
};

