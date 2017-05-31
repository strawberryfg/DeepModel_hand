#pragma once

#include <windows.h>

#include <string>
using namespace std;

unsigned int GlutKey2WinKey(unsigned int key);
string Key2Str(unsigned int key);

enum 
{
	KEY_BACKSPACE		= 8
	,KEY_TAB			= 9
	,KEY_ENTER			= 13
	,KEY_SHIFT			= 16
	,KEY_CTRL			= 17
	,KEY_BREAK			= 19
	,KEY_ESC			= 27
	,KEY_SPACE			= 32
	,KEY_PAGE_UP		= 33
	,KEY_PAGE_DOWN		= 34
	,KEY_END			= 35
	,KEY_HOME			= 36
	,KEY_LEFT_ARROW		= 37
	,KEY_UP_ARROW		= 38
	,KEY_RIGHT_ARROW	= 39
	,KEY_DOWN_ARROW		= 40
	,KEY_INSERT			= 45
	,KEY_DELETE			= 46
	,KEY_NUMBER_0		= 48
	,KEY_CHAR_A			= 65
	,KEY_WINDOWS		= 91
	,KEY_CONTEXT_MENU	= 93
	,KEY_CHAR_a			= 97
	,KEY_MULTIPLY		= 106
	,KEY_PLUS			= 107
	,KEY_MINUS			= 109
	,KEY_DIVIDE			= 111
	,KEY_F1				= 112
	,KEY_F2				= 113
	,KEY_F3				= 114
	,KEY_F4				= 115
	,KEY_F5				= 116
	,KEY_F6				= 117
	,KEY_F7				= 118
	,KEY_F8				= 119
	,KEY_F9				= 120
	,KEY_F10			= 121
	,KEY_F11			= 122
	,KEY_F12			= 123
	,KEY_DEFAULT = 65535
};

inline bool IsCtrlPressed()
{
	return (GetKeyState(VK_LCONTROL) < 0) || (GetKeyState(VK_RCONTROL) < 0);
}

inline bool IsShiftPressed()
{
	return (GetKeyState(VK_LSHIFT) < 0) || (GetKeyState(VK_RSHIFT) < 0);
}

// those defined in glut
enum
{
	GLUT_KEY_BACKSPACE = KEY_BACKSPACE
	,GLUT_KEY_TAB = KEY_TAB
	,GLUT_KEY_ENTER = KEY_ENTER	
	,GLUT_KEY_ESC = KEY_ESC
	,GLUT_KEY_SPACE = KEY_SPACE

	,GLUT_KEY_NUMBER_0 = KEY_NUMBER_0
	,GLUT_KEY_CHAR_A = KEY_CHAR_A
	,GLUT_KEY_CHAR_a =  KEY_CHAR_a

	,GLUT_KEY_MULTIPLY = '*'
	,GLUT_KEY_PLUS = '+'
	,GLUT_KEY_MINUS = '-'
	,GLUT_KEY_DIVIDE = '/'
};

// those defined in opencv
enum 
{
	CV_KEY_BACKSPACE	= 8
	,CV_KEY_TAB			= 9
	,CV_KEY_ENTER		= 13
	,CV_KEY_ESC			= 27
	,CV_KEY_SPACE		= 32

	,CV_KEY_PAGE_UP		= 2162688
	,CV_KEY_PAGE_DOWN	= 2228224
	,CV_KEY_END			= 2293760
	,CV_KEY_HOME		= 2359296
	,CV_KEY_LEFT_ARROW	= 2424832
	,CV_KEY_UP_ARROW	= 2490368
	,CV_KEY_RIGHT_ARROW	= 2555904
	,CV_KEY_DOWN_ARROW	= 2621440
	,CV_KEY_INSERT		= 2949120
	,CV_KEY_DELETE		= 3014656

	,CV_KEY_CHAR_a		= 97
	,CV_KEY_CHAR_A		= 65
	,CV_KEY_MULTIPLY	= 42
	,CV_KEY_PLUS		= 43
	,CV_KEY_MINUS		= 45
	,CV_KEY_DIVIDE		= 47

	,CV_KEY_F1			= 7340032
	,CV_KEY_F2			= 7405568
	,CV_KEY_F3			= 7471104
	,CV_KEY_F4			= 7536640
	,CV_KEY_F5			= 7602176
	,CV_KEY_F6			= 7667712
	,CV_KEY_F7			= 7733248
	,CV_KEY_F8			= 7798784
	,CV_KEY_F9			= 7864320
	,CV_KEY_F10			= 7929856
	,CV_KEY_F11			= 7995392
	,CV_KEY_F12			= 8060928
};