#include "KeyboardHelper.h"

#include <map>

map<unsigned char, string> key_2_str_table;
void PrepareKey2Str(map<unsigned char, string>& table)
{
	table[KEY_BACKSPACE] = "BackSpace";
	table[KEY_TAB] = "Tab";
	table[KEY_ENTER] = "Enter";
	table[KEY_SHIFT] = "Shift";
	table[KEY_CTRL] = "Ctrl";
	table[KEY_BREAK] = "Break";
	table[KEY_ESC] = "Esc";
	table[KEY_SPACE] = "Space";
	table[KEY_PAGE_UP] = "PageUp";
	table[KEY_PAGE_DOWN] = "PageDown";
	table[KEY_END] = "End";
	table[KEY_HOME] = "Home";
	table[KEY_LEFT_ARROW] = "Left";
	table[KEY_UP_ARROW] = "Up";
	table[KEY_RIGHT_ARROW] = "Right";
	table[KEY_DOWN_ARROW] = "Down";
	table[KEY_INSERT] = "Insert";
	table[KEY_DELETE] = "Delete";
	table[KEY_WINDOWS] = "Windows";
	table[KEY_CONTEXT_MENU] = "ContextMenu";
	table[KEY_MULTIPLY] = "Multiply";
	table[KEY_PLUS] = "Plus";
	table[KEY_MINUS] = "Minus";
	table[KEY_DIVIDE] = "Divide";
	table[KEY_F1] = "F1";
	table[KEY_F2] = "F2";
	table[KEY_F3] = "F3";
	table[KEY_F4] = "F4";
	table[KEY_F5] = "F5";
	table[KEY_F6] = "F6";
	table[KEY_F7] = "F7";
	table[KEY_F8] = "F8";
	table[KEY_F9] = "F9";
	table[KEY_F10] = "F10";
	table[KEY_F11] = "F11";
	table[KEY_F12] = "F12";
	
	table[KEY_DEFAULT] = "Default";

	char num[2] = "0";
	for(int n = 0; n < 10; n++, num[0]++)
	{
		table[KEY_NUMBER_0+n] = num;
	}

	char lower[2] = "a", upper[2] = "A";
	for(int n = 0; n < 26; n++, lower[0]++, upper[0]++)
	{
		table[KEY_CHAR_a+n] = lower;
		table[KEY_CHAR_A+n] = upper;
	}
}

map<unsigned int, unsigned int> glutkey_2_winkey_table;
void PrepareGlutKey2WinKey(map<unsigned int, unsigned int>& table)
{
	table[GLUT_KEY_BACKSPACE] = KEY_BACKSPACE;
	table[GLUT_KEY_TAB] = KEY_TAB;
	table[GLUT_KEY_ENTER] = KEY_ENTER;
	table[GLUT_KEY_ESC] = KEY_ESC;
	table[GLUT_KEY_SPACE] = KEY_SPACE;

	table[GLUT_KEY_MULTIPLY] = KEY_MULTIPLY;
	table[GLUT_KEY_PLUS] = KEY_PLUS;
	table[GLUT_KEY_MINUS] = KEY_MINUS;
	table[GLUT_KEY_DIVIDE] = KEY_DIVIDE;

	unsigned int glut_num = GLUT_KEY_NUMBER_0;
	unsigned int win_num = KEY_NUMBER_0;
	for(int n = 0; n < 10; n++)
		table[glut_num++] = win_num++;
	
	unsigned int lower_glut = GLUT_KEY_CHAR_a;
	unsigned int lower_win = KEY_CHAR_a;
	unsigned int upper_glut = GLUT_KEY_CHAR_A;
	unsigned int upper_win = KEY_CHAR_A;
	for(int n = 0; n < 26; n++)
	{
		table[lower_glut++] = lower_win++;
		table[upper_glut++] = upper_win++;
	}
}

class KeyboardHelper
{
	static KeyboardHelper s_helper;

	KeyboardHelper() 
	{
		PrepareGlutKey2WinKey(glutkey_2_winkey_table);
		PrepareKey2Str(key_2_str_table);
	}
};

KeyboardHelper KeyboardHelper::s_helper;

unsigned int GlutKey2WinKey(unsigned int key)
{
	map<unsigned int, unsigned int>::iterator it = glutkey_2_winkey_table.find(key);
	
	if (it != glutkey_2_winkey_table.end())
		return it->second;
	else return KEY_DEFAULT;
}

string Key2Str(unsigned int key)
{
	map<unsigned char, string>::iterator it = key_2_str_table.find(key);
	if (it != key_2_str_table.end())
		return it->second;
	else return "Undefined";
}