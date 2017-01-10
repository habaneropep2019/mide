//=============================================================================
// File:       exampl02.cpp
// Contents:   Source code for Example 2 -- Parsing a simple message
// Maintainer: Doug Sauder <dwsauder@fwb.gulf.net>
// WWW:        http://www.fwb.gulf.net/~dwsauder/mimepp.html
// $Revision: 1.6 $
// $Date: 1997/09/27 11:55:40 $
//
// Copyright (c) 1996, 1997 Douglas W. Sauder
// All rights reserved.
// 
// IN NO EVENT SHALL DOUGLAS W. SAUDER BE LIABLE TO ANY PARTY FOR DIRECT,
// INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF
// THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF DOUGLAS W. SAUDER
// HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// DOUGLAS W. SAUDER SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT
// NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
// PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS"
// BASIS, AND DOUGLAS W. SAUDER HAS NO OBLIGATION TO PROVIDE MAINTENANCE,
// SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
//
//=============================================================================

#include <stdlib.h>
#include <iostream.h>
#include <fstream.h>
#include "basicmsg.h"

#include <mimelib/token.h>


int main()
{
    // Initialize the library

    DwInitialize();

    // Read message from file

    DwString messageStr = "";
    DwString line;
    ifstream istrm("exampl02.txt");
    while (DwTrue) {
        getline(istrm, line);
        if (istrm.eof()) {
            break;
        }
        messageStr += line + DW_EOL;
    }
    istrm.close();
    
    // Create a DwMessage and parse it.  The DwMessage should be created on
    // the free store, since it will be added to the BasicMessage.

    DwMessage* msg = DwMessage::NewMessage(messageStr, 0);
    msg->Parse();

    // Create a Message and add the DwMessage to it

    BasicMessage message(msg);

    // Open file stream for output

    ofstream ostrm("exampl02.out");

    // Print the header fields

    ostrm << "Type -> " << message.TypeStr() << "\n";
    ostrm << "Subtype -> " << message.SubtypeStr() << "\n";
    ostrm << "Content-Transfer-Encoding -> " << message.CteStr() << "\n";
    ostrm << "Date -> " << message.DateStr() << "\n";
    ostrm << "From -> " << message.From() << "\n";
    ostrm << "To -> " << message.To() << "\n";
    ostrm << "Cc -> " << message.Cc() << "\n";
    ostrm << "Bcc -> " << message.Bcc() << "\n";
    ostrm << "Subject -> " << message.Subject() << "\n";

    // Print the body

    ostrm << "\nBody ->\n";
    ostrm << message.Body() << "\n";

    return 0;
}

