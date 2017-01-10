#include <qimage.h>
#include <kabapi.h>
#include <iostream.h>
#include <kapp.h>
#include <kimgio.h>

int main(int argc, char** argv)
{
  // ############################################################################
  KApplication app(argc, argv);
  KabAPI::ErrorCode ec;
  AddressBook::Entry entry;
  entry.name="Test Test Test";
  list<AddressBook::Entry> entries;
  string key, selection;
  kimgioRegister();
  KabAPI api;
  // -----
  cout << "main: starting." << endl;
  switch(ec=api.init(false))
    {
    case KabAPI::NoError: 
      break;
    case KabAPI::PermDenied: 
      cout << "main: Permission for r/w mode denied, file is locked." << endl;
      break;
    default:
      cout << "main: initialization error." << endl;
      return -1;
    }
  if(api.add(entry, key)==KabAPI::NoError)
    {
      cout << "main: entry added, key is " << key 
	   << "." << endl;
    } else {
      cout << "main: no entry added." << endl;
    }
  if(api.getEntries(entries)==KabAPI::NoError)
    {
      cout << "main: " << entries.size() 
	   << " entries retrieved from database." << endl;
    } else {
      cout << "main: error retrieving all entries at once." 
	<< endl;
    }
  if(api.exec())
    {
      cout << "main: accepted." << endl;
      if((ec=api.getEntry(entry, selection))!=KabAPI::NoError)
	{
	  switch(ec)
	    {
	    case KabAPI::NoEntry: 
	      cout << "main: there is no entry." << endl;
	      break;
	    default: 
	      cout << "main: unknown error." << endl;
	    }
	} else {
	  cout << "main: the selected entry is "
	       << entry.firstname << " " << entry.name 
	       << ", its key is " << key << "." << endl;
	}
    } else {
      cout << "main: rejected." << endl;
    }
  if(!key.empty() && api.remove(key)==KabAPI::NoError)
    {
      cout << "main: entry " << key << " removed." << endl;
    } else {
      cout << "main: entry " << key << "not removed." << endl;
    }
  // -----
  return 0;
  // ############################################################################
}


