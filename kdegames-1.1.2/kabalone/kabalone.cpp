/* Start point for KAbablone */

#include <qobject.h>
#include "AbTop.h"

int main(int argc, char *argv[])
{
	KApplication app(argc, argv, "kabalone");
	AbTop* top = new AbTop;
	int res;

	if (app.isRestored())
	  top->restore(1);
	else {
	  top->readConfig();
	  top->show();
	}
	       
	res = app.exec();

	delete top;

	return res;
}

