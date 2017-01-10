//=============================================================================
// File:       msgcmp.cpp
// Contents:   Definitions for DwMessageComponent
// Maintainer: Doug Sauder <dwsauder@fwb.gulf.net>
// WWW:        http://www.fwb.gulf.net/~dwsauder/mimepp.html
// $Revision: 1.7 $
// $Date: 1997/09/27 11:54:08 $
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

#define DW_IMPLEMENTATION

#include <mimelib/config.h>
#include <mimelib/debug.h>
#include <stdlib.h>
#include <mimelib/msgcmp.h>

#define kMagicNumber ((DwUint32) 0x22222222L)


int DwMessageComponent::sNextObjectId = 1;
const char* const DwMessageComponent::sClassName = "DwMessageComponent";


DwMessageComponent::DwMessageComponent()
{
    mMagicNumber = (DwUint32) kMagicNumber;
    mObjectId = sNextObjectId++;
    mIsModified = 0;
    mParent = 0;
    mClassId = kCidMessageComponent;
    mClassName = sClassName;
}


DwMessageComponent::DwMessageComponent(const DwMessageComponent& aCmp)
  : mString(aCmp.mString)
{
    mMagicNumber = (DwUint32) kMagicNumber;
    mObjectId = sNextObjectId++;
    mIsModified = aCmp.mIsModified;
    mParent = 0;
    mClassId = kCidMessageComponent;
    mClassName = sClassName;
}


DwMessageComponent::DwMessageComponent(const DwString& aStr,
    DwMessageComponent* aParent)
  : mString(aStr)
{
    mMagicNumber = (DwUint32) kMagicNumber;
    mObjectId = sNextObjectId++;
    mIsModified = 0;
    mParent = aParent;
    mClassId = kCidMessageComponent;
    mClassName = sClassName;
}


DwMessageComponent::~DwMessageComponent()
{
#if defined (DW_DEBUG_VERSION) || defined (DW_DEVELOPMENT_VERSION)
    if (mMagicNumber != (DwUint32) kMagicNumber) {
        cerr << "Bad value for 'this' in destructor\n";
        cerr << "(Possibly 'delete' was called twice for same object)\n";
        abort();
    }
    mMagicNumber = 0;
#endif // defined (DW_DEBUG_VERSION) || defined (DW_DEVELOPMENT_VERSION)
}


const DwMessageComponent&
DwMessageComponent::operator = (const DwMessageComponent& aCmp)
{
    if (this == &aCmp) return *this;
    mString = aCmp.mString;
    mIsModified = aCmp.mIsModified;
    return *this;
}


void DwMessageComponent::FromString(const DwString& aStr)
{
    mString = aStr;
    if (mParent != 0) {
        mParent->SetModified();
    }
}


void DwMessageComponent::FromString(const char* aCstr)
{
    assert(aCstr != 0);
    mString = aCstr;
    if (mParent != 0) {
        mParent->SetModified();
    }
}


const DwString& DwMessageComponent::AsString()
{
    return mString;
}


DwMessageComponent* DwMessageComponent::Parent()
{
    return mParent;
}


void DwMessageComponent::SetParent(DwMessageComponent* aParent)
{
    mParent = aParent;
}


DwBool DwMessageComponent::IsModified() const
{
    return mIsModified;
}


void DwMessageComponent::SetModified()
{
    mIsModified = 1;
    if (mParent) mParent->SetModified();
}


int DwMessageComponent::ClassId() const
{
    return mClassId;
}


const char* DwMessageComponent::ClassName() const
{
    return mClassName;
}


int DwMessageComponent::ObjectId() const
{
    return mObjectId;
}


void DwMessageComponent::PrintDebugInfo(ostream& aStrm, int /*aDepth*/) const
{
#if defined (DW_DEBUG_VERSION)
    _PrintDebugInfo(aStrm);
#endif // defined (DW_DEBUG_VERSION)
}


void DwMessageComponent::_PrintDebugInfo(ostream& aStrm) const
{
#if defined (DW_DEBUG_VERSION)
    aStrm << "ObjectId:         " << ObjectId() << '\n';
    aStrm << "ClassId:          ";
	switch (ClassId()) {
	case kCidError:
		aStrm << "kCidError";
		break;
	case kCidUnknown:
        aStrm << "kCidUnknown";
		break;
	case kCidAddress:
        aStrm << "kCidAddress";
		break;
	case kCidAddressList:
        aStrm << "kCidAddressList";
		break;
	case kCidBody:
        aStrm << "kCidBody";
		break;
	case kCidBodyPart:
        aStrm << "kCidBodyPart";
		break;
	case kCidDispositionType:
        aStrm << "kCidDispositionType";
		break;
	case kCidMechanism:
        aStrm << "kCidMechanism";
		break;
	case kCidMediaType:
        aStrm << "kCidMediaType";
		break;
	case kCidParameter:
        aStrm << "kCidParameter";
		break;
	case kCidDateTime:
        aStrm << "kCidDateTime";
		break;
	case kCidEntity:
        aStrm << "kCidEntity";
		break;
	case kCidField:
        aStrm << "kCidField";
		break;
	case kCidFieldBody:
        aStrm << "kCidFieldBody";
		break;
	case kCidGroup:
        aStrm << "kCidGroup";
		break;
	case kCidHeaders:
        aStrm << "kCidHeaders";
		break;
	case kCidMailbox:
        aStrm << "kCidMailbox";
		break;
	case kCidMailboxList:
        aStrm << "kCidMailboxList";
		break;
	case kCidMessage:
        aStrm << "kCidMessage";
		break;
	case kCidMessageComponent:
        aStrm << "kCidMessageComponent";
		break;
	case kCidMsgId:
        aStrm << "kCidMsgId";
		break;
	case kCidText:
        aStrm << "kCidText";
		break;
	}
    aStrm << '\n';
    aStrm << "ClassName:        " << ClassName() << '\n';
    aStrm << "String:           " << mString << '\n';
    aStrm << "IsModified:       " << (IsModified() ? "True" : "False") << '\n';
    aStrm << "Parent ObjectId:  ";
    if (mParent) {
        aStrm << mParent->ObjectId() << '\n';
    }
    else {
        aStrm << "(none)\n";
    }
#endif // defined (DW_DEBUG_VERSION)
}


void DwMessageComponent::CheckInvariants() const
{
#if defined (DW_DEBUG_VERSION)
    assert(mMagicNumber == kMagicNumber);
    assert(mClassName != 0);
    mString.CheckInvariants();
#endif // defined (DW_DEBUG_VERSION)
}


