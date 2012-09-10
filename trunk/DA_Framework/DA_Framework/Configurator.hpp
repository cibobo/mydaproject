#ifndef TINY_PARSER_HPP
#define TINY_PARSER_HPP

#include <fstream>  
#include <iostream>  
#include <sstream>  
#include <string>

#include "MainThread.hpp"

using namespace std;

class TinyParser
{
public:
	TinyParser(void);
	~TinyParser(void);

	// Line parsers for configuration file
	void parseLineDate( std::string line, int* date );	
	void parseLineDimensions( std::string line, int* dim );
	void parseLineIP( std::string line, int* ip );
	void parseLineInteger( std::string line, int* val );
	void parseLineFloat( std::string line, float* val );
	void parseLineDouble( std::string line, double* val );

	//MEMORY LEAK
	int parseLineInteger(std::string line);
	float parseLineFloat(std::string line);
	double parseLineDouble(std::string line);
};


class Configurator {
public:
	Configurator(MainThread *p);
	~Configurator();

	int readConfigurationFile();

	// Path of the configuration file
	string strConfigureFile;
	// Daten type of the parameters
	string strParserConfiguration;
	// Parser
	TinyParser tp;

	// Pointer of MainThread
	MainThread *pMainThread;

	string modelNames;
};

#endif 