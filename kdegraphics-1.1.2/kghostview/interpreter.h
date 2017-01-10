#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <qdialog.h>
#include <qpushbt.h>
#include <qchkbox.h>
#include <qgrpbox.h>
#include <qlined.h>
#include <qradiobt.h>
#include <qbttngrp.h>

enum {COLOR_PALETTE, GRAY_PALETTE, MONO_PALETTE};
enum {PIX_BACKING, STORE_BACKING};

class  InterpreterDialog : public QDialog {
    Q_OBJECT
    
public:
    InterpreterDialog ( QWidget* parent = 0, const char* name = 0 );
    ~InterpreterDialog();
    
	void init();
    void setDefaultValues();
	
	bool antialias;
	bool show_messages;
	bool platform_fonts;
	int paletteOpt;
	int backingOpt;

public slots:

	void setValues();
	void setDefaults();

protected slots:


protected:
    QButtonGroup* backingGroup;
    QButtonGroup* paletteGroup;
    QGroupBox* generalBox;
    QLineEdit* intEdit;
    QCheckBox* antialiasBox;
    QCheckBox* messageBox;
    QCheckBox* fontsBox;
    QRadioButton* colorButton;
    QRadioButton* grayButton;
    QRadioButton* monoButton;
    QRadioButton* pixButton;
    QRadioButton* storeButton;
    QPushButton* okButton;
    QPushButton* defaultButton;
    QPushButton* cancelButton;
};

#endif 
