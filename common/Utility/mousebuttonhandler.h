#pragma once

#include <string>
#include <vector>
using namespace std;

#define MOUSE_LEFT_BUTTON 0
#define MOUSE_MIDDLE_BUTTON 1
#define MOUSE_RIGHT_BUTTON 2

class CMouseButtonHandler
{
public:
	virtual string Name() const { return "a virtual base class for button handler"; }

	virtual void OnDown(int button, int x, int y) 
	{
		down_x = x;	down_y = y;	last_x = x;	last_y = y;	is_dragging[button] = true;	
	}

	virtual void OnUp(int button, int x,int y)	
	{	
		is_dragging[button] = false;	
	}

	virtual void OnMove(int x, int y)
	{
		if (is_dragging[MOUSE_LEFT_BUTTON])
			OnDrag(MOUSE_LEFT_BUTTON, x, y);
		else if (is_dragging[MOUSE_MIDDLE_BUTTON])
			OnDrag(MOUSE_MIDDLE_BUTTON, x, y);
		else if (is_dragging[MOUSE_RIGHT_BUTTON])
			OnDrag(MOUSE_RIGHT_BUTTON, x, y);
		else
			OnHover(x, y);
	}

	virtual void OnDrag(int button, int x, int y)	
	{	
		last_x = x;	last_y = y;	
	}

	virtual void OnHover(int x, int y)	{}

	virtual string GetDescription() const = 0;
	
	virtual bool OnKeyDown(unsigned int key)	{	return false;	}	// return whether the key is handled
	virtual bool GetKeyDescription(vector<string>& desc) const = 0;		// return false if not any key is handled

protected:
	CMouseButtonHandler()
	{
		is_dragging[0] = is_dragging[1] = is_dragging[2] = false;
	}

	// the dragging flag is different for three buttons, but position is the same
	bool is_dragging[3];
	int down_x, down_y;		// mouse position when button is pressed
	int last_x, last_y;		// mouse position in moving
};