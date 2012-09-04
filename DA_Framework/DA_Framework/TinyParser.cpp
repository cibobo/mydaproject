#include "stdafx.h"
#include "TinyParser.hpp"

//#include <fstream>  
//#include <iostream>  
#include <sstream>  
//#include <vector> 
using namespace std;

TinyParser::TinyParser(void){};
TinyParser::~TinyParser(void){};

// Parsing configuration files for dates
void TinyParser::parseLineDate( std::string line, int* date )
{
	int index = 0;
	int old_pos = 0;
	int pos = 0;
	while ( (pos != -1) && (index < 2) )
	{
		pos = (int)line.find(".", pos+1);
		if (pos != -1) 
		{
			istringstream temp(line.substr( old_pos, pos-old_pos ));						
			temp >> date[index];
			index++;
			pos++;
			old_pos = pos;
		}
	}
	pos = (int)line.rfind(".");
	istringstream temp(line.substr( pos+1, line.size() - pos ));						
	temp >> date[index];
}

// Parsing configuration files for image dimensions 
void TinyParser::parseLineDimensions( std::string line, int* dim )
{
	int index = 0;
	int old_pos = 0;
	int pos = 0;
	while ( (pos != -1) && (index < 1) )
	{
		pos = (int)line.find(" ", pos+1);
		if (pos != -1) 
		{
			istringstream temp(line.substr( old_pos, pos-old_pos ));						
			temp >> dim[index];
			index++;
			pos++;
			old_pos = pos;
		}
	}
	pos = (int)line.rfind(" ");
	istringstream temp(line.substr( pos+1, line.size() - pos ));						
	temp >> dim[index];
}

// Parsing configuration files for ip adress 
void TinyParser::parseLineIP( std::string line, int* ip )
{
	int index = 0;
	int old_pos = 0;
	int pos = 0;
	while ( (pos != -1) && (index < 3) )
	{
		pos = (int)line.find(".", pos+1);
		if (pos != -1) 
		{
			istringstream temp(line.substr( old_pos, pos-old_pos ));						
			temp >> ip[index];
			index++;
			pos++;
			old_pos = pos;
		}
	}
	pos = (int)line.rfind(".");
	istringstream temp(line.substr( pos+1, line.size() - pos ));						
	temp >> ip[index];
}

// Parsing configuration files for integer value 
void TinyParser::parseLineInteger( std::string line, int* val )
{
	istringstream temp(line);						
	temp >> val[0];
}
// Parsing configuration files for float value 
void TinyParser::parseLineFloat( std::string line, float* val )
{
	istringstream temp(line);						
	temp >> val[0];
}
// Parsing configuration files for double value 
void TinyParser::parseLineDouble( std::string line, double* val )
{
	istringstream temp(line);						
	temp >> val[0];
}


//MEMORY LEAK
// Parsing configuration files for integer value 
int TinyParser::parseLineInteger( std::string line )
{
	istringstream temp(line);						
	int i;
	temp >> i;
	return i;
}
// Parsing configuration files for float value 
float TinyParser::parseLineFloat( std::string line )
{
	istringstream temp(line);						
	float f;
	temp >> f;
	return f;
	//temp >> val[0];
}
// Parsing configuration files for double value 
double TinyParser::parseLineDouble( std::string line)
{
	istringstream temp(line);						
	double d;
	temp >> d;
	return d;
}