//FSME: Finite State Machine Engine
//

#include <exception>
#include <map>

using namespace std;

namespace fsm {

//---------------
/* fsme ----------------
	Virtual base state of fsme
--------------------------------*/
class fsme {
private:
public:
	 fsme() { }
virtual ~fsme() { }

virtual void	Go(void*) const = 0;
};

//---------------
/* bot ----------------
	FSME Robot
--------------------------------*/
template<typename Event>
 class bot:
	  public fsme
	, public basic_istream<Event>
	, public basic_ostream<Event> {
private:
	typedef Event		event_type;

Event	state;

public:
map<Event, void(*)(Event)> handler;

	 bot () { }
virtual	~bot () { }

//	void	Handler(Event e, void(func*)() ) const { handler[e] = func; }
	void	Go(void*) const { }
};


}//namespace fsm
