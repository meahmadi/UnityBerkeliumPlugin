// Copyright (c) 2010 Jeroen Dierckx - Expertise Centre for Digital Media. All Rights reserved.
// This source code is developed for the flemish (Belgian) OSMA project (http://osma.phl.be/)
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Includes
#include "UnityBerkeliumWindow.h"

//float lookup table, so we read float value based on index (0-255) rather than calculating float value
static const float lookup[]=
{
         0.0f,  0.00392157f,  0.00784314f,   0.0117647f,   0.0156863f,   0.0196078f,   0.0235294f,    0.027451f,
   0.0313726f,   0.0352941f,   0.0392157f,   0.0431373f,   0.0470588f,   0.0509804f,    0.054902f,   0.0588235f,
   0.0627451f,   0.0666667f,   0.0705882f,   0.0745098f,   0.0784314f,   0.0823529f,   0.0862745f,   0.0901961f,
   0.0941177f,   0.0980392f,    0.101961f,    0.105882f,    0.109804f,    0.113725f,    0.117647f,    0.121569f,
     0.12549f,    0.129412f,    0.133333f,    0.137255f,    0.141176f,    0.145098f,     0.14902f,    0.152941f,
    0.156863f,    0.160784f,    0.164706f,    0.168627f,    0.172549f,    0.176471f,    0.180392f,    0.184314f,
    0.188235f,    0.192157f,    0.196078f,         0.2f,    0.203922f,    0.207843f,    0.211765f,    0.215686f,
    0.219608f,    0.223529f,    0.227451f,    0.231373f,    0.235294f,    0.239216f,    0.243137f,    0.247059f,
     0.25098f,    0.254902f,    0.258824f,    0.262745f,    0.266667f,    0.270588f,     0.27451f,    0.278431f,
    0.282353f,    0.286275f,    0.290196f,    0.294118f,    0.298039f,    0.301961f,    0.305882f,    0.309804f,
    0.313726f,    0.317647f,    0.321569f,     0.32549f,    0.329412f,    0.333333f,    0.337255f,    0.341176f,
    0.345098f,     0.34902f,    0.352941f,    0.356863f,    0.360784f,    0.364706f,    0.368627f,    0.372549f,
    0.376471f,    0.380392f,    0.384314f,    0.388235f,    0.392157f,    0.396078f,         0.4f,    0.403922f,
    0.407843f,    0.411765f,    0.415686f,    0.419608f,    0.423529f,    0.427451f,    0.431373f,    0.435294f,
    0.439216f,    0.443137f,    0.447059f,     0.45098f,    0.454902f,    0.458824f,    0.462745f,    0.466667f,
    0.470588f,     0.47451f,    0.478431f,    0.482353f,    0.486275f,    0.490196f,    0.494118f,    0.498039f,
    0.501961f,    0.505882f,    0.509804f,    0.513726f,    0.517647f,    0.521569f,     0.52549f,    0.529412f,
    0.533333f,    0.537255f,    0.541176f,    0.545098f,     0.54902f,    0.552941f,    0.556863f,    0.560784f,
    0.564706f,    0.568627f,    0.572549f,    0.576471f,    0.580392f,    0.584314f,    0.588235f,    0.592157f,
    0.596078f,            0.6f,    0.603922f,    0.607843f,    0.611765f,    0.615686f,    0.619608f,    0.623529f,
    0.627451f,    0.631373f,    0.635294f,    0.639216f,    0.643137f,    0.647059f,     0.65098f,    0.654902f,
    0.658824f,    0.662745f,    0.666667f,    0.670588f,     0.67451f,    0.678431f,    0.682353f,    0.686275f,
    0.690196f,    0.694118f,    0.698039f,    0.701961f,    0.705882f,    0.709804f,    0.713726f,    0.717647f,
    0.721569f,     0.72549f,    0.729412f,    0.733333f,    0.737255f,    0.741176f,    0.745098f,     0.74902f,
    0.752941f,    0.756863f,    0.760784f,    0.764706f,    0.768628f,    0.772549f,    0.776471f,    0.780392f,
    0.784314f,    0.788235f,    0.792157f,    0.796079f,            0.8f,    0.803922f,    0.807843f,    0.811765f,
    0.815686f,    0.819608f,    0.823529f,    0.827451f,    0.831373f,    0.835294f,    0.839216f,    0.843137f,
    0.847059f,     0.85098f,    0.854902f,    0.858824f,    0.862745f,    0.866667f,    0.870588f,     0.87451f,
    0.878431f,    0.882353f,    0.886275f,    0.890196f,    0.894118f,    0.898039f,    0.901961f,    0.905882f,
    0.909804f,    0.913726f,    0.917647f,    0.921569f,     0.92549f,    0.929412f,    0.933333f,    0.937255f,
    0.941177f,    0.945098f,     0.94902f,    0.952941f,    0.956863f,    0.960784f,    0.964706f,    0.968628f,
    0.972549f,    0.976471f,    0.980392f,    0.984314f,    0.988235f,    0.992157f,    0.996078f,         1.0f,
};

/******************************
* Constructors and destructor *
******************************/

UnityBerkeliumWindow::UnityBerkeliumWindow(int uniqueID, float *buffer, int width, int height, const string &url)
: m_id(uniqueID), m_buffer(buffer), m_width(width), m_height(height), m_url(url)
{
	assert(m_buffer);
	assert(width > 0 && height > 0);

	m_pWindow = Berkelium::Window::create();
	m_pWindow->setDelegate(this);
	m_pWindow->resize(width, height);
	m_pWindow->navigateTo(url.data(), url.length());
  cerr << "--- Opening url: " << url.data() << endl;
  //m_pWindow->setTransparent(false);
}

UnityBerkeliumWindow::~UnityBerkeliumWindow()
{
	delete m_pWindow;
}


/******************
* Other functions *
******************/

/**************************************
* Berkelium::WindowDelegate functions *
**************************************/

bool UnityBerkeliumWindow::getDirty() {  return isDirty;  }
void UnityBerkeliumWindow::setDirty(bool dirtyBool)  {    isDirty = dirtyBool;  }
void UnityBerkeliumWindow::onPaint(Berkelium::Window *pWindow, const unsigned char *sourceBuffer, const Berkelium::Rect &rect, int dx, int dy, const Berkelium::Rect &scrollRect)
{
	cerr << "[UnityBerkeliumWindow] onPaint called (window: " << pWindow << ")" << endl;
	cerr << "  rect: (left=" << rect.left() << ", width=" << rect.width() << ", top=" << rect.top() <<  ", height=" << rect.height() << ")"  << endl;
	cerr << "  sourceBuffer: " << (void *) sourceBuffer << " (size: " << sizeof(sourceBuffer) << ")" << endl;

  // Scrolling

	if(dx != 0 || dy != 0)
	{

	}

  tempRect.mLeft = rect.left();
  tempRect.mTop = rect.top();
  tempRect.mWidth = rect.width();
  tempRect.mHeight = rect.height();
    
  float val; //our alpha value

  int shift = 0; // shift value to move between pixels

	for(int x = rect.left(); x < rect.right(); ++x)
	{

		for(int y = rect.top(); y < rect.bottom(); ++y)
		{

			m_buffer[shift + 0] = lookup[sourceBuffer[shift + 2]]; //R
			m_buffer[shift + 1] = lookup[sourceBuffer[shift + 1]]; //G
			m_buffer[shift + 2] = lookup[sourceBuffer[shift + 0]]; //B
      
          if((
            m_buffer[shift + 0] == 1.0f // Blue value pure blue?
            && m_buffer[shift + 1] == 0.0f // Green value 0?
            && m_buffer[shift + 2]) == 1.0f //Red value pure red?
            ) // is the pixel value hot pink?
          {
            val = 0.0f;  // then set to transparent
          }
          else
          { 
            val = 1.0f; // then set to opaque
          } 

      //val = ((m_buffer[shift + 0] + m_buffer[shift + 1] + m_buffer[shift + 2]) / 3) + -1; //<----------average alpha based on greyscale

     // val =  lookup[sourceBuffer[shift + 3]]; //<----------- true alpha value, not assigned

      m_buffer[shift + 3] = val;  // apply the value to the alpha slot of the pixel

      shift += 4; // shift our index to the next pixel

		}
	}
  setDirty(true); // pixel has been updated, notify Unity to redraw this frame
}

void UnityBerkeliumWindow::onAddressBarChanged(Berkelium::Window *win, const char* newURL, size_t newURLSize)
{
	cerr << "[UnityBerkeliumWindow] onAddressBarChanged called (window: " << win << ")" << endl;
}

void UnityBerkeliumWindow::onStartLoading(Berkelium::Window *win, const char *newURL, size_t newURLSize)
{
	cerr << "[UnityBerkeliumWindow] onStartLoading called (window: " << win << ")" << endl;
}

void UnityBerkeliumWindow::onLoad(Berkelium::Window *win)
{
	cerr << "[UnityBerkeliumWindow] onLoad called (window: " << win << ")" << endl;
}

void UnityBerkeliumWindow::onLoadError(Berkelium::Window *win, const char* error, size_t errorLength)
{
	cerr << "[UnityBerkeliumWindow] onLoadError called (window: " << win << ")" << endl;
}

void UnityBerkeliumWindow::onBeforeUnload(Berkelium::Window *win, bool *proceed)
{
	cerr << "[UnityBerkeliumWindow] onBeforeUnload called (window: " << win << ")" << endl;
}

void UnityBerkeliumWindow::onCancelUnload(Berkelium::Window *win)
{
	cerr << "[UnityBerkeliumWindow] onCancelUnload called (window: " << win << ")" << endl;
}

void UnityBerkeliumWindow::onCrashed(Berkelium::Window *win)
{
	cerr << "[UnityBerkeliumWindow] onCrashed called (window: " << win << ")" << endl;
}

void UnityBerkeliumWindow::onUnresponsive(Berkelium::Window *win)
{
	cerr << "[UnityBerkeliumWindow] onUnresponsive called (window: " << win << ")" << endl;
}

void UnityBerkeliumWindow::onResponsive(Berkelium::Window *win)
{
	cerr << "[UnityBerkeliumWindow] onResponsive called (window: " << win << ")" << endl;
}

void UnityBerkeliumWindow::onChromeSend(Berkelium::Window *win, Data message, const Data* content, size_t numContents)
{
	cerr << "[UnityBerkeliumWindow] onChromeSend called (window: " << win << ")" << endl;
}

void UnityBerkeliumWindow::onCreatedWindow(Berkelium::Window *win, Berkelium::Window *newWindow)
{
	cerr << "[UnityBerkeliumWindow] onCreatedWindow called (window: " << win << ")" << endl;
}

void UnityBerkeliumWindow::onWidgetCreated(Berkelium::Window *win, Berkelium::Widget *newWidget, int zIndex)
{
	cerr << "[UnityBerkeliumWindow] onWidgetCreated called (widget: " << newWidget << ")" << endl;
}

void UnityBerkeliumWindow::onWidgetDestroyed(Berkelium::Window *win, Berkelium::Widget *newWidget)
{
	cerr << "[UnityBerkeliumWindow] onWidgetDestroyed called (widget: " << newWidget << ")" << endl;
}

void UnityBerkeliumWindow::onWidgetResize(Berkelium::Window *win, Berkelium::Widget *wid, int newWidth,int newHeight)
{
	cerr << "[UnityBerkeliumWindow] onWidgetResize called (widget: " << wid << ")" << endl;
}

void UnityBerkeliumWindow::onWidgetMove(Berkelium::Window *win, Berkelium::Widget *wid, int newX, int newY)
{
	cerr << "[UnityBerkeliumWindow] onWidgetMove called (widget: " << wid << ")" << endl;
}

void UnityBerkeliumWindow::onWidgetPaint(Berkelium::Window *win, Berkelium::Widget *wid, const unsigned char *sourceBuffer, const Berkelium::Rect &rect,int dx, int dy, const Berkelium::Rect &scrollRect)
{
	cerr << "[UnityBerkeliumWindow] onWidgetPaint called (widget: " << wid << ")" << endl;
}

	
/**********************
* Protected functions *
**********************/

