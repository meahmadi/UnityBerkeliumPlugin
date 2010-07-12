// Copyright (c) 2010 Jeroen Dierckx - Expertise Centre for Digital Media. All Rights reserved.
// This source code is developed for the flemish (Belgian) OSMA project (http://osma.phl.be/)
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "global.h"
#include "UnityBerkeliumWindow.h"
#include <map>

static const char *logFilename = "BerkeliumUnityTest.log";

// We reference-count the Berkelium backend
static int refCount = 0;

// A map that holds the created windows
typedef std::map<int, UnityBerkeliumWindow *> WindowMap;
static WindowMap windows;

UnityBerkeliumWindow *getWindow(int id)
{
	WindowMap::const_iterator it = windows.find(id);
	return (it == windows.end()) ? 0 : it->second;
}


/********************
* Exposed functions *
********************/

PLUGIN_API void Berkelium_init()
{
	if(refCount == 0)
	{
		// Initialize the Berkelium engine
		Berkelium::init();

		// Redirect stderr to log file
		::freopen(logFilename, "w", stderr);
	}

	++refCount;
}

PLUGIN_API void Berkelium_destroy()
{
	--refCount;

	if(refCount == 0)
	{
		// Destroy the windows that still exist
		for(WindowMap::iterator it = windows.begin(); it != windows.end(); ++it)
			delete it->second;
		windows.clear();

		// Close the berkelium backend
		Berkelium::destroy();

		// Close the log file
		fclose(stderr);
	}
}


PLUGIN_API void Berkelium_update(int windowID)
{
	UnityBerkeliumWindow *pWindow = getWindow(windowID);

  //set dirty to false, if it needs changing, onPaint will change it to true
  pWindow->setDirty(false);

	//! @todo We need to call this only once, not for each object
	Berkelium::update();
}

PLUGIN_API bool Berkelium_Window_create(int uniqueID, float *colors, int width, int height, char *url) // changed void* to float* since we know we're passing floats
{
	if(windows.find(uniqueID) != windows.end())
	{
		cerr << "Error: a berkelium window with ID " << uniqueID << " already exists!" << endl;
		return false;
	}

	// Unity uses floats for colors
	float *data = reinterpret_cast<float *>(colors);

	UnityBerkeliumWindow *pWindow = new UnityBerkeliumWindow(uniqueID, data, width, height, url);

	cerr << "Berkelium window created: " << pWindow << " (size=" << width << ", " << height << "; url=" << url << ")" << endl;
	windows[uniqueID] = pWindow;

	return true;
}

PLUGIN_API void Berkelium_Window_destroy(int windowID)
{
	WindowMap::iterator it = windows.find(windowID);
	if(it != windows.end())
	{
		delete it->second;
		windows.erase(it);
	}
}

PLUGIN_API void Berkelium_Window_mouseDown(int windowID, int button)
{
	UnityBerkeliumWindow *pWindow = getWindow(windowID);
	if(pWindow)
		pWindow->getBerkeliumWindow()->mouseButton(button, true);
	else
		cerr << "Warning: no berkelium window found with ID " << windowID << "!" << endl;
}

PLUGIN_API void Berkelium_Window_mouseUp(int windowID, int button)
{
	UnityBerkeliumWindow *pWindow = getWindow(windowID);
	if(pWindow)
		pWindow->getBerkeliumWindow()->mouseButton(button, false);
	else
		cerr << "Warning: no berkelium window found with ID " << windowID << "!" << endl;
}

PLUGIN_API void Berkelium_Window_mouseMove(int windowID, int x, int y)
{
	UnityBerkeliumWindow *pWindow = getWindow(windowID);
	if(pWindow)
		pWindow->getBerkeliumWindow()->mouseMoved(x, y);
	else
		cerr << "Warning: no berkelium window found with ID " << windowID << "!" << endl;
}

PLUGIN_API void Berkelium_Window_textEvent(int windowID, char c){

	UnityBerkeliumWindow *pWindow = getWindow(windowID);
	if(pWindow){
		
        wchar_t outchars[2]; // the structure for passing chars
        outchars[0] = c; // the character we want to inject
        outchars[1] = 0; //terminating character
        pWindow->getBerkeliumWindow()->textEvent(outchars,1);

	}else{
		cerr << "Warning: no berkelium window found with ID " << windowID << "!" << endl;
	}
}

PLUGIN_API void Berkelium_Window_keyEvent(int windowID, int key){

	UnityBerkeliumWindow *pWindow = getWindow(windowID);
	if(pWindow){

		bool pressed = true; 
        int wvmods = 0; //mapGLUTModsToBerkeliumMods(glutGetModifiers());
        int vk_code = key; // this is used for backspace as it cannot be passed as char
        int scancode = 0;

       pWindow->getBerkeliumWindow()->keyEvent(pressed, wvmods, vk_code, scancode);   

	}else{
		cerr << "Warning: no berkelium window found with ID " << windowID << "!" << endl;
	}
}

PLUGIN_API int Berkelium_Window_rectLeft(int windowID){
	UnityBerkeliumWindow *pWindow = getWindow(windowID);
	if(pWindow){
    return pWindow->tempRect.left();
	}else{
		cerr << "Warning: no berkelium window found with ID " << windowID << "!" << endl;
    return 0;
	}
}
PLUGIN_API int Berkelium_Window_rectTop(int windowID){
	UnityBerkeliumWindow *pWindow = getWindow(windowID);
	if(pWindow){
    return pWindow->tempRect.top();
	}else{
		cerr << "Warning: no berkelium window found with ID " << windowID << "!" << endl;
    return 0;
	}
}
PLUGIN_API int Berkelium_Window_rectHeight(int windowID){
	UnityBerkeliumWindow *pWindow = getWindow(windowID);
	if(pWindow){
    return pWindow->tempRect.height();
	}else{
		cerr << "Warning: no berkelium window found with ID " << windowID << "!" << endl;
    return 0;
	}
}
PLUGIN_API int Berkelium_Window_rectWidth(int windowID){
	UnityBerkeliumWindow *pWindow = getWindow(windowID);
	if(pWindow){
    return pWindow->tempRect.width();
	}else{
		cerr << "Warning: no berkelium window found with ID " << windowID << "!" << endl;
    return 0;
	}
}
PLUGIN_API bool Berkelium_Window_isDirty(int windowID){
	UnityBerkeliumWindow *pWindow = getWindow(windowID);
	if(pWindow){
    return pWindow->getDirty();
	}else{
		cerr << "Warning: no berkelium window found with ID " << windowID << "!" << endl;
    return false;
	}
}