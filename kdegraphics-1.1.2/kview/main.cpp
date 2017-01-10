
#include "kview.h"

int main( int argc, char *argv[] )
{
	KView *kview = new KView( argc, argv );

	int ret = kview->exec();

	delete kview;

	return ret;
}
