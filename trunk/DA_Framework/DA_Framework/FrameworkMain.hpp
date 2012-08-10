#include "process.h" 
#include "resource.h"
#include "stdio.h" 
#include "windows.h" 
#include "string"

#include <iostream>
#include <list>

#include "BildData.hpp"
#include "DistanceFilter.hpp"
#include "MyDataIO.hpp"
#include "OpenGLDraw.hpp"


using namespace std;

// The main function of the framework
void FrameworkMain();

// Announcement of threads
void offlineInputThreadProc(void *param);

void openGLSceneThreadPorc(void *param);