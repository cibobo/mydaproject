#include "stdafx.h"
#include "Object.hpp"

Object::Object(){
}

Object::Object(std::vector<Point2f> points){
	this->markers = points;
}

Object::~Object(){
}

void Object::addNewMarker(Point2f point){
	this->markers.insert(markers.end(), point);
}

void Object::addNewMarkers(vector<Point2f> points){
	this->markers.insert(markers.end(), points.begin(), points.end());
}

int Object::getSize(){
	return this->markers.size();
}