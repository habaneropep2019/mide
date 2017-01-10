
#include<unistd.h>
#include<kapp.h>

#include"kornset.h"
#include"korn.h"

int main(int argc, char *argv[])
{
	KApplication *kornApp= new KApplication(argc, argv, "korn");
	KornSettings *settings= new KornSettings(kornApp->getConfig(), 60);
	Korn *korn = new Korn(0, settings);

	kornApp->setMainWidget( korn );

	delete settings;

	if( korn->hasConfig() ) {
		korn->show();
	}

	kornApp->exec();

	delete korn;
	delete kornApp;

	return 0;
}
