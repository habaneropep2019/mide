#include "kbiff.h"
#include <kwm.h>
#include <kapp.h>

#include "setupdlg.h"
#include "Trace.h"

int main(int argc, char *argv[])
{
	KApplication app(argc, argv, "kbiff");
	KBiff kbiff;
	KBiffSetup* setup = 0;
	bool is_secure    = false;
	bool have_profile = false;
	QString profile;

	app.setMainWidget(&kbiff);

	if (argc > 1)
	{
		for (int i = 1; i < argc; i++)
		{
			QString arg(argv[i]);
			if (arg == "-debug")
				TRACEON(true);
			else
			if (arg == "-secure")
				is_secure = true;
			else
			if (arg == "-profile")
			{
				// make sure there is at least one more arg
				if ((i + 1) <= argc)
				{
					profile = argv[i+1];
					have_profile = true;
					i++;
				}
			}
		}
	}

TRACEINIT("main()");

	// restore this app if it is
	if (kapp->isRestored())
	{
		TRACE("isRestored()");
		kbiff.readSessionConfig();
	}
	else
	{
		TRACE("notRestored()");
		// do we have the profile option?
		if (have_profile)
			setup = new KBiffSetup(profile, is_secure);
		else
		{
			setup = new KBiffSetup();
			if (!setup->exec())
			{
				delete setup;
				return 0;
			}
		}
		kbiff.processSetup(setup, true);
	}

	// check if we are docked (only if restored)
	if (kbiff.isDocked())
	{
		kapp->setTopWidget(new QWidget);
		KWM::setDockWindow(kbiff.winId());
	}
	else
		kapp->setTopWidget(&kbiff);

	// enable session management
	kapp->enableSessionManagement();

	kbiff.show();

	return app.exec();
}
