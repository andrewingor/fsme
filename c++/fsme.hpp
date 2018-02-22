//FSME: Finite State Machine Engine
//

#include <exception>
#include <vector>
#include <string>
#include <map>

using namespace std;

namespace fsm {

//---------
/* engine ----------
	Base of FSMe
--------------------------------*/
template<  typename __iEvent = uint8_t
	 , typename __oEvent = __iEvent >
 class engine:
	  public basic_istream<__iEvent>
	, public basic_ostream<__oEvent> {
private:
	//
public:
	typedef	__iEvent	event_in_type;

	 engine() { }
virtual ~engine() { }

const string&	rev() const {
		static string Rev = ("$Id$"); return Rev; }

virtual void	go(__iEvent) = 0;
};

//----------
/* state --------
	FSM State
--------------------------------*/
template< typename __iEvent = uint8_t
	, typename __oEvent = __iEvent >
 class state:
	  public engine<__iEvent, __oEvent>{
private:
	//
public:
	//typedef void(fsm::state<__iEvent, __oEvent>::*Handle)();
	//typedef	map<__iEvent, Handle>	___Handles;
	typedef fsm::state<__iEvent, __oEvent>*	___State;
	typedef __iEvent	event_in_type;
	typedef __oEvent	event_out_type;

___State	Current;

public:
	state() { }
virtual	~state(){ }

virtual	void	go(__iEvent) = 0;
	void	transit(___State Next) { Current = Next; }
	
};

//----------
/* init --------------------------
	State of FSMe, and example
-----------------------------------*/
class init:
	public state<>{
private:
	// -------- H A N D L E S
static void	zero(void*_Data) { clog << "INIT::ZERO Data=3" << endl; *((int*)_Data) = 3; }
static void	one (void*_Data) { clog << "INIT::ONE  Data=1" << endl; *((int*)_Data) = 1; }
	//void	two(uint8_t*) { clog << "Two" << endl; }
	//void	thr(uint8_t*) { clog << "Three" << endl; }
	// -------- H A N D L E S
	//
	typedef void(*Handle)(void*);
	typedef	map<uint8_t, Handle>	___Handles;

___Handles	__Process;
int		__Data = 0;

public:
	init() {
		state::Current = 0;
		__Process = {
		  { 0, zero }
		, { 1, one  }
		};
	}
virtual	~init() { }

virtual	void	go(uint8_t Event) { (*__Process[Event])(&__Data);  }
virtual const int& data() const { return __Data; } 
};

}//namespace fsm
