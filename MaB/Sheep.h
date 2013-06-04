#ifndef SHEEP_H
#define SHEEP_H

#include "Entity.h"
#include "StateMachine.h"

class Sheep : public Entity
{
	StateMachine<Sheep>*  stateMachine;

public:
	Sheep( Game& g) : Entity(g)
	{
		Strength(3);
		pos = vel = Vector(0,0);
		speed = 1.f;
		dim = Vector(32,32);
		fovDim = Vector(50, 50);

		stateMachine = new StateMachine<Sheep>(this);
		stateMachine->SetCurrentState(WanderNS::Instance());
		//m_pStateMachine->SetGlobalState(SheepsGlobalState::Instance());
	}
	~Sheep()
	{
		delete stateMachine;
	}

	void Logic() override 
	{
		stateMachine->Update();
		Entity::Logic();
	}

	StateMachine<Sheep>* GetFSM() const { return stateMachine; }

	// sheep states
	class WanderNS : public State<Sheep>
	{

	private:
  
	  WanderNS(){}

	  //copy ctor and assignment should be private
	  WanderNS(const WanderNS&);
	  WanderNS& operator=(const WanderNS&); 
  
	public:

	  static WanderNS* Instance()
	  {
		  static WanderNS instance;

		  return &instance;
	  }
  
	  virtual void Enter(Sheep* sheep) {}

	  virtual void Execute(Sheep* sheep)
	  {
		  if ( !sheep->isMoving() )
		  {
			  if ( rand()%2 == 1 ) {
				  sheep->GetFSM()->SetCurrentState( WanderWE::Instance() );
			  }
			  else {
				  if ( rand()%2 == 1 )
					sheep->MoveTiles( 0, -1);
				  else
					sheep->MoveTiles( 0, 1);
			  }
		  }
	  }

	  virtual void Exit(Sheep* sheep) {}

	};

	class WanderWE : public State<Sheep>
	{
	private:
  
	  WanderWE(){}

	  //copy ctor and assignment should be private
	  WanderWE(const WanderWE&);
	  WanderWE& operator=(const WanderWE&);
 
	public:

	  static WanderWE* Instance() 
	  {
		  static WanderWE instance;

		  return &instance;
	  }
  
	  virtual void Enter(Sheep* sheep) {}

	  virtual void Execute(Sheep* sheep)
	  {
		  if ( !sheep->isMoving() )
		  {
			  if ( rand()%2 == 1 ) {
				  sheep->GetFSM()->SetCurrentState( WanderNS::Instance() );
			  }
			  else {
				  if ( rand()%2 == 1 )
					sheep->MoveTiles( -1, 0);
				  else
					sheep->MoveTiles( 1, 0);
			  }
		  }
	  }

	  virtual void Exit(Sheep* sheep) {}

	};
};



#endif