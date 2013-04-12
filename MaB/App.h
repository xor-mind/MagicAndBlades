#ifndef _APP_H
#define _APP_H

class App {
    protected:
		bool running;
 
    public:
		App() : running(true) {}
		virtual ~App() {}
 
		int Execute()
		{
			if( Init() == false) {
				return -1;
			}

			GameLoop();
 
			Cleanup();
 
			return 0;
		}

	protected:
		virtual void GameLoop() {}
 
		virtual bool Init() { return true; }
 
		virtual void Logic() {}
 
		virtual void Render() {}
 
		virtual void Cleanup() {}
};

#endif