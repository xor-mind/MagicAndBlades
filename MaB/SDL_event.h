#ifndef SDL_EVENT_H
#define SDL_EVENT_H

#define VIRT virtual
#define VV VIRT void

#include <SDL.h>

class Event 
{
public:
	Event() {}
 
	VIRT ~Event() {}
 
	VV OnEvent(SDL_Event* Event) 
	{
		switch(Event->type) 
		{
			case SDL_ACTIVEEVENT: 
			{
				switch(Event->active.state) 
				{
					case SDL_APPMOUSEFOCUS: 
					{
						if ( Event->active.gain )    
							MouseFocus();
						else                
							MouseBlur();
					} break;

					case SDL_APPINPUTFOCUS: 
					{
						if ( Event->active.gain )    
							InputFocus();
						else                
							InputBlur();	
					} break;
						
					case SDL_APPACTIVE:    
					{
						if ( Event->active.gain )    
							Restore();
						else                
							Minimize();   
					} break;
				}   
			} break;
 
			case SDL_KEYDOWN: 
			{
				KeyDown(Event->key.keysym.sym,Event->key.keysym.mod,Event->key.keysym.unicode);		
			} break;
 
			case SDL_KEYUP: 
			{
				KeyUp(Event->key.keysym.sym,Event->key.keysym.mod,Event->key.keysym.unicode);				
			} break;
 
			case SDL_MOUSEMOTION: 
			{
				MouseMove(Event->motion.x,Event->motion.y,Event->motion.xrel,Event->motion.yrel,(Event->motion.state&SDL_BUTTON(SDL_BUTTON_LEFT))!=0,(Event->motion.state&SDL_BUTTON(SDL_BUTTON_RIGHT))!=0,(Event->motion.state&SDL_BUTTON(SDL_BUTTON_MIDDLE))!=0);				
			} break;
 
			case SDL_MOUSEBUTTONDOWN: 
			{
				switch(Event->button.button) 
				{
					case SDL_BUTTON_LEFT: {
						LButtonDown(Event->button.x,Event->button.y);						
					} break;
					case SDL_BUTTON_RIGHT: {
						RButtonDown(Event->button.x,Event->button.y);						
					} break;
					case SDL_BUTTON_MIDDLE: {
						MButtonDown(Event->button.x,Event->button.y);					
					} break;
				} break;
			}
 
			case SDL_MOUSEBUTTONUP:    
			{
				switch(Event->button.button) 
				{
					case SDL_BUTTON_LEFT: {
						LButtonUp(Event->button.x,Event->button.y);						
					} break;
					case SDL_BUTTON_RIGHT: {
						RButtonUp(Event->button.x,Event->button.y);						
					} break;
					case SDL_BUTTON_MIDDLE: {
						MButtonUp(Event->button.x,Event->button.y);					
					} break;
				} break;
			}
 
			case SDL_JOYAXISMOTION: 
			{
				JoyAxis(Event->jaxis.which,Event->jaxis.axis,Event->jaxis.value);				
			} break;
 
			case SDL_JOYBALLMOTION: 
			{
				JoyBall(Event->jball.which,Event->jball.ball,Event->jball.xrel,Event->jball.yrel);				
			} break;
 
			case SDL_JOYHATMOTION: 
			{
				JoyHat(Event->jhat.which,Event->jhat.hat,Event->jhat.value);				
			} break;

			case SDL_JOYBUTTONDOWN: 
			{
				JoyButtonDown(Event->jbutton.which,Event->jbutton.button);				
			} break;
 
			case SDL_JOYBUTTONUP:
			{
				JoyButtonUp(Event->jbutton.which,Event->jbutton.button);   
			} break;
 
			case SDL_QUIT: 
			{
				Exit();		
			} break;
 
			case SDL_SYSWMEVENT: {
				//Ignore
			} break;
 
			case SDL_VIDEORESIZE: {
				Resize(Event->resize.w,Event->resize.h);			
			} break;
 
			case SDL_VIDEOEXPOSE: {
				Expose();		
			} break;
 
			default: {
				User(Event->user.type,Event->user.code,Event->user.data1,Event->user.data2);
            } break;
		}
	}
 
	VV InputFocus() {
		//Pure virtual, do nothing
	}
 
	VV InputBlur() {
		//Pure virtual, do nothing
	}
 
	VV KeyDown(SDLKey sym, SDLMod mod, Uint16 unicode) {
		//Pure virtual, do nothing
	}
 
	VV KeyUp(SDLKey sym, SDLMod mod, Uint16 unicode) {
		//Pure virtual, do nothing
	}
 
	VV MouseFocus() {
		//Pure virtual, do nothing
	}
 
	VV MouseBlur() {
		//Pure virtual, do nothing
	}
 
	VV MouseMove(int mX, int mY, int relX, int relY, bool Left,bool Right,bool Middle) {
		//Pure virtual, do nothing
	}
 
	VV MouseWheel(bool Up, bool Down) {
		//Pure virtual, do nothing
	}
 
	VV LButtonDown(int mX, int mY) {
		//Pure virtual, do nothing
	}
 
	VV LButtonUp(int mX, int mY) {
		//Pure virtual, do nothing
	}
 
	VV RButtonDown(int mX, int mY) {
		//Pure virtual, do nothing
	}
 
	VV RButtonUp(int mX, int mY) {
		//Pure virtual, do nothing
	}
 
	VV MButtonDown(int mX, int mY) {
		//Pure virtual, do nothing
	}
 
	VV MButtonUp(int mX, int mY) {
		//Pure virtual, do nothing
	}
 
	VV JoyAxis(Uint8 which,Uint8 axis,Sint16 value) {
		//Pure virtual, do nothing
	}
 
	VV JoyButtonDown(Uint8 which,Uint8 button) {
		//Pure virtual, do nothing
	}
 
	VV JoyButtonUp(Uint8 which,Uint8 button) {
		//Pure virtual, do nothing
	}
 
	VV JoyHat(Uint8 which,Uint8 hat,Uint8 value) {
		//Pure virtual, do nothing
	}
 
	VV JoyBall(Uint8 which,Uint8 ball,Sint16 xrel,Sint16 yrel) {
		//Pure virtual, do nothing
	}
 
	VV Minimize() {
		//Pure virtual, do nothing
	}
 
	VV Restore() {
		//Pure virtual, do nothing
	}
 
	VV Resize(int w,int h) {
		//Pure virtual, do nothing
	}
 
	VV Expose() {
		//Pure virtual, do nothing
	}
 
	VV Exit() {
		//Pure virtual, do nothing
	}
 
	VV User(Uint8 type, int code, void* data1, void* data2) {
		//Pure virtual, do nothing
	}
};

#endif