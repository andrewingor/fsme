//
//

#include <stdlib.h>
#include <stdio.h>

typedef void(*Handle)(void);

Handle begin = NULL;
Handle **TableOf;

static int Event = 0, State = 0;

//
void Input(
) {
	Event = getchar();
	if (EOF == Event) exit(0);
	else
		goto *TableOf[State][Event];
}

//
void hand1(
) {
	printf("HANDLE\n");
	goto *begin;
}

//
static Handle state1[] = { Input, hand1 };
static Handle *table[] = { state1 };

//
int main( int	argc
	, char* argv[]
) {
	begin = Input;
	TableOf = table;

	goto *TableOf[0][0];
}

