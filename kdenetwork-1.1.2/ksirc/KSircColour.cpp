/**********************************************************************

 Colour Setter for the text colours in TopLevel list box.

 GUI done with qtarch.

 Nothing special, just pops up a box with buttons, you click on them
 and up comes a colours selector.  Pretty basic.

 Only strange things is the ammount of dorking with pointers, you have
 to set the value os pointed to item, not change the pointer, hence
 the *ptr notation everywhere.

 *********************************************************************/

#include "KSircColour.h"
#include "config.h"

#include <kconfig.h>

#define Inherited KSircColourData

extern KConfig *kConfig;

KSircColour::KSircColour
(
	QWidget* parent,
	const char* name
)
	:
	Inherited( parent, name )
{
	setCaption( i18n("Colour Settings") );

	// Set the text colour for the 4 main buttons.
	// Get the pointer values and off you go.

	ColourText->setColor( *kSircConfig->colour_text );
	ColourInfo->setColor( *kSircConfig->colour_info );
	ColourChan->setColor( *kSircConfig->colour_chan );
	ColourError->setColor( *kSircConfig->colour_error );
	if(kSircConfig->colour_background == 0){
	  kConfig->setGroup("Colours");
	  kSircConfig->colour_background = new("QColor") QColor(kConfig->readColorEntry("Background", new("QColor") QColor(colorGroup().mid())));
	}
	ColourBackground->setColor( *kSircConfig->colour_background );

	connect(ColourText, SIGNAL(changed(const QColor &)),
		this, SLOT(colourChange(const QColor &)));
	connect(ColourInfo, SIGNAL(changed(const QColor &)),
		this, SLOT(colourChange(const QColor &)));
	connect(ColourChan, SIGNAL(changed(const QColor &)),
		this, SLOT(colourChange(const QColor &)));
	connect(ColourError, SIGNAL(changed(const QColor &)),
		this, SLOT(colourChange(const QColor &)));
	connect(ColourBackground, SIGNAL(changed(const QColor &)),
		this, SLOT(colourChange(const QColor &)));
}


KSircColour::~KSircColour()
{
}

void KSircColour::ok()
{

  // Write the values back again.  This will change the painter 
  // colours on the fly.
  
  *kSircConfig->colour_text = ColourText->color();
  *kSircConfig->colour_info = ColourInfo->color();
  *kSircConfig->colour_chan = ColourChan->color();
  *kSircConfig->colour_error = ColourError->color();
  *kSircConfig->colour_background = ColourBackground->color();
  close(1);
  kConfig->setGroup("Colours");
  kConfig->writeEntry("text", *kSircConfig->colour_text);
  kConfig->writeEntry("info", *kSircConfig->colour_info);
  kConfig->writeEntry("chan", *kSircConfig->colour_chan);
  kConfig->writeEntry("error", *kSircConfig->colour_error);
  kConfig->writeEntry("Background", *kSircConfig->colour_background);
  kConfig->sync();

}

void KSircColour::cancel()
{
  close(1);
}

void KSircColour::colourChange(const QColor &)
{
  *kSircConfig->colour_text = ColourText->color();
  *kSircConfig->colour_info = ColourInfo->color();
  *kSircConfig->colour_chan = ColourChan->color();
  *kSircConfig->colour_error = ColourError->color();
  *kSircConfig->colour_background = ColourBackground->color();
  emit update();
}
#include "KSircColour.moc"
