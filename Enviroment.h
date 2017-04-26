#pragma once
#include"simulationParam.h"
#include"nodeStruct.h"
#include"obstacle.h"
#include<list>
#include"SMP.h"
#include"Robot.h"
#include"RRTstar.h"
//--------------------------------------------------------------class
class Enviroment
{
public:
	//--------------------------------------------------------------Function
	// Default constructor  
	Enviroment() {};
	// Default destructor  
	~Enviroment() {};
	// Setup method
	void setup();
	// Update method
	void update();
	// Render method draw nodes in enviroment.
	void render();

	void renderGrid();
	//--------------------------------------------------------------Variables
	bool grid = false;
private:
	//--------------------------------------------------------------Variables
protected:
	//--------------------------------------------------------------Variables
	std::list<Nodes> nodes;
	std::list<obstacles> obst;
	std::list<Nodes> path;
	RRTstar rrtstar;
	bool rrtFlag = true;
	bool planner = false;
	bool vechicle = false;

	ofVec2f goal;


	ofVec2f home;
	Robot car;

	// A list or an array of Obstacles should come here

};

inline void Enviroment::setup()
{
	home.set(startx, starty);
	car.setup(home);

	for (unsigned int i = 0; i < numberOfobst; i++)
	{
		obstacles ob;
		obst.push_back(ob);
	}

	Nodes start(startx, starty, 0);
	this->nodes.push_back(start);
	goal.set(goalx, goaly);
	SMP::goal = goal;
	SMP::goalFound = false;
}

inline void Enviroment::update()
{
	/*std::list<obstacles>::iterator itobs = obst.begin();
	while (itobs != obst.end()) {
		itobs->move();
		itobs++;
	}*/

	rrtstar.nextIter(nodes, obst);
	/*
	if (rrtFlag) {
		std::list<Nodes>::iterator it = nodes.begin();
		while (it != nodes.end())
		{
			if (it->location.distance(goal) < converge)
			{
				rrtFlag = !rrtFlag;
				target = &(*it);
			}
			it++;
		}
	}*/

	if (SMP::goalFound && !planner)
	{
		path.clear();
		Nodes *pathNode = SMP::target;
		do
		{
			path.push_back(*pathNode);
			pathNode = pathNode->parent;
		} while (pathNode->parent != NULL);
		planner = !planner;
		path.reverse();
	}

	if (planner && !vechicle) {
		std::list<Nodes>::iterator pathIt = path.begin();

		while(pathIt !=path.end()){
			if (pathIt->alive) {
				car.controller(pathIt->location);
				float dist= pathIt->location.distance(car.getLocation());
				if (dist < 2.0) pathIt->alive = false;
				break;
			}
			pathIt++;
		}
		if(pathIt!=path.end())
		car.update();

	}
}

inline void Enviroment::render()
{
	ofEnableAlphaBlending();
	for (auto i : obst) {
		i.render();
		//cout << i.getX() << "  " << i.getY() << endl;
	}


	ofSetColor({255, 255, 10});
	ofFill();
	ofDrawCircle(goal.x, goal.y, NODE_RADIUS);
	ofNoFill();
	ofDrawCircle(goal.x, goal.y, converge);

	for (auto i : this->nodes)
	{
		ofSetColor({ 10,10,10 });

		if (i.parent != NULL && i.prevParent == NULL) {
			ofPoint pt;ofPolyline line;
			pt.set(i.location.x, i.location.y);line.addVertex(pt);
			pt.set(i.parent->location.x, i.parent->location.y);line.addVertex(pt);
			line.draw();
		}
		ofSetColor({ 10,250,250 });
		if (i.prevParent != NULL) {
			
			ofPoint pt; ofPolyline line;
			pt.set(i.location.x, i.location.y); line.addVertex(pt);
			pt.set(i.prevParent->location.x, i.prevParent->location.y); line.addVertex(pt);
			line.draw();
		}
		int hue = i.alive ? 130 : 80;
		ofSetColor(i.color, hue);
		//ofDrawCircle(i.location.x, i.location.y, NODE_RADIUS);
	}
	if (!path.empty())
	{
		ofSetColor({ 20,250,30 });
		ofSetLineWidth(5);
		for (auto i : path) {
			if (i.parent != NULL) {
				ofPoint pt; ofPolyline line;
				pt.set(i.location.x, i.location.y); line.addVertex(pt);
				pt.set(i.parent->location.x, i.parent->location.y); line.addVertex(pt);
				line.draw();
			}
		}
		ofSetLineWidth(1);
	}
	car.render();
	ofDisableAlphaBlending();
}

//inline void Enviroment::render()
//{
//	if (grid == true) renderGrid();
//	ofEnableAlphaBlending();
//	for (size_t i = 0; i < TotalNodes; i++)
//	{
//		float x = ofRandom(0, ofGetWindowWidth());
//		float y = ofRandom(0, ofGetWindowHeight());
//
//		Nodes node; 
//
//		node.location.set(x,y);
//		int hue = node.alive ? 130 : 80;
//		ofSetColor(node.color, hue);
//
//		ofDrawCircle(node.location.x, node.location.y, NODE_RADIUS);
//	}
//
//	ofDisableAlphaBlending();
//}

inline void Enviroment::renderGrid()
{
	ofEnableAlphaBlending();
	ofSetColor(20, 130, 0, 50);
	for (int i = 0; i < ofGetWindowWidth(); i += 5)
	{
		ofDrawLine(i, 0, i, ofGetWindowHeight());
	}
	for (int j = 0; j < ofGetWindowHeight(); j += 5)
	{
		ofDrawLine(0, j, ofGetWindowWidth(), j);
	}
	ofDisableAlphaBlending();
}