/***************************************************************************
                          msnprotocol.cpp  -  description
                             -------------------
    begin                : Wed Jan 2 2002
    copyright            : (C) 2002 by duncan
    email                : duncan@tarro
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <kdebug.h>
#include <kconfig.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <klocale.h>
#include "msnprotocol.h"
#include "msncontact.h"
#include <msnadd.h>
#include "kopete.h"
#include <msnaddcontactpage.h>

///////////////////////////////////////////////////
//           Constructor & Destructor
///////////////////////////////////////////////////

MSNProtocol::MSNProtocol(): QObject(0, "MSN"), IMProtocol()
{
	connected = 0;
	// Remember to move all this to init()

	kdDebug() << "\nMSN Plugin Loading\n";
	// Load all ICQ icons from KDE standard dirs
 	initIcons();
	
	kdDebug() << "MSN Protocol Plugin: Creating Status Bar icon\n";
	statusBarIcon = new StatusBarIcon();
	
	kdDebug() << "MSN Protocol Plugin: Setting icon offline\n";
	statusBarIcon->setPixmap(&offlineIcon);

	kdDebug() << "MSN Protocol Plugin: Creating Config Module\n";
	new MSNPreferences(protocolIcon, this);
	
	kdDebug() << "MSN Protocol Plugin: Creating MSN Engine\n";
	engine = new KMSNService;
	connect(engine, SIGNAL(connectedToService(bool)), this, SLOT(slotConnectedToMSN(bool)));
	connect(engine, SIGNAL(contactStatusChanged(QString, QString, int)), this, SIGNAL(userStateChange (QString, QString, int) ) );
	//connect(engine, SIGNAL(userStateChange (QString, QString, QString)), this, SLOT(slotUserStateChange (QString, QString, QString) ) );
	//connect(engine, SIGNAL(userStateChange (QString, QString, QString)), this, SLOT(slotInitContacts(QString, QString, QString) ) );
	//connect(engine, SIGNAL(userSetOffline (QString) ), this, SLOT(slotUserSetOffline(QString) ) );
	// Someone unknown talk to us
	connect(engine, SIGNAL( newContact(QString) ), this, SLOT(slotAuthenticate(QString) ) );
	
	kdDebug() << "MSN Protocol Plugin: Done\n";

	KGlobal::config()->setGroup("MSN");

	/** Autoconnect if is selected in config */
	if ( KGlobal::config()->readBoolEntry("AutoConnect", "0") )
	{
		Connect();
	}
	
}

MSNProtocol::~MSNProtocol()
{

}

///////////////////////////////////////////////////
//           Plugin Class reimplementation
///////////////////////////////////////////////////

void MSNProtocol::init()
{

}

bool MSNProtocol::unload()
{
	kdDebug() << "MSN Protocol: Unloading...\n";
	kopeteapp->statusBar()->removeWidget(statusBarIcon);
	delete statusBarIcon;
	// heh!
	return 1;
}

///////////////////////////////////////////////////
//           IMProtocol Class reimplementation
///////////////////////////////////////////////////

void MSNProtocol::Connect()
{
	if ( !isConnected() )
	{
		KGlobal::config()->setGroup("MSN");
		kdDebug() << "Attempting to connect to MSN" << endl;
		kdDebug() << "Setting Monopoly mode..." << endl;
		kdDebug() << "Using Micro$oft UserID " << KGlobal::config()->readEntry("UserID", "0") << " with password " << KGlobal::config()->readEntry("Password", "") << endl;
		KGlobal::config()->setGroup("MSN");

		engine->setMyContactInfo( KGlobal::config()->readEntry("UserID", "")
								, KGlobal::config()->readEntry("Password", ""));
		//engine->setMyPublicName(KGlobal::config()->readEntry("Nick", ""));
		engine->connectToService();
	}
	else
	{
    	kdDebug() << "MSN Plugin: Ignoring Connect request (Already Connected)" << endl;
	}	
}

void MSNProtocol::Disconnect()
{
	if ( isConnected() )
	{
		engine->disconnect();
	}
	else
	{
    	kdDebug() << "MSN Plugin: Ignoring Disconnect request (Im not Connected)" << endl;
	}	
}


bool MSNProtocol::isConnected()
{
	return connected;	
}

/** This i used for al protocol selection dialogs */
QPixmap MSNProtocol::getProtocolIcon()
{
	return protocolIcon;
}

AddContactPage *MSNProtocol::getAddContactWidget(QWidget *parent)
{
	return (new MSNAddContactPage(this,parent));	
}

///////////////////////////////////////////////////
//           Internal functions implementation
///////////////////////////////////////////////////

/** No descriptions */
void MSNProtocol::initIcons()
{
	KIconLoader *loader = KGlobal::iconLoader();

	protocolIcon = QPixmap(loader->loadIcon("msn_protocol", KIcon::User));
	onlineIcon = QPixmap(loader->loadIcon("msn_online", KIcon::User));
	offlineIcon = QPixmap(loader->loadIcon("msn_offline", KIcon::User));
	awayIcon = QPixmap(loader->loadIcon("msn_away", KIcon::User));
	naIcon = QPixmap(loader->loadIcon("msn_na", KIcon::User));
}

/** OK! We are connected , let's do some work */
void MSNProtocol::slotConnected()
{
 	MSNContact *tmpcontact;
		
 	QStringList groups, contacts;
 	QString group, publicname, userid;
 	uint status;
 	// First, we change status bar icon
 	statusBarIcon->setPixmap(&onlineIcon);
     // We get the group list
 	groups = engine->getGroups();
 	for ( QStringList::Iterator it = groups.begin(); it != groups.end(); ++it )
 	{
 		//item=  new QListViewItem(ListView,(*it).latin1() ,"","1");
 		//item->setPixmap(0,expandedPixmap);
 		//item->setOpen(true);
	
 		// We get the contacts for this group
 		contacts = engine->getContacts( (*it).latin1() );
 		for ( QStringList::Iterator it1 = contacts.begin(); it1 != contacts.end(); ++it1 )
 	 	{
 	 		userid = (*it1).latin1();
 			publicname = engine->getPublicName((*it1).latin1());
 			tmpcontact = new MSNContact( userid , publicname , this );
 			//item1= new QListViewItem(item, engine->getPublicName((*it1).latin1())  , (*it1).latin1() ,"1");
 	 		status = engine->getStatus( userid );
 	 		kdDebug() << "MSN Plugin: Created contact " << userid << " " << publicname << " with status " << status << endl;
			switch(status)
 			{
  				case NLN:
  				{
  					tmpcontact->setPixmap(0, onlineIcon);
  					break;
  				}
  				case FLN:
  				{
  					tmpcontact->setPixmap(0, offlineIcon);
  					break;
  				}
  				case BSY:
  				{
  					tmpcontact->setPixmap(0, onlineIcon);
  					break;
  				}
  				case IDL:
  				{
  					tmpcontact->setPixmap(0, onlineIcon);
  					break;
  				}
  				case AWY:
  				{
  					tmpcontact->setPixmap(0, awayIcon);
  					break;
  				}
  				case PHN:
  				{
  					tmpcontact->setPixmap(0, onlineIcon);
  					break;
  				}
  				case BRB:
  				{
  					tmpcontact->setPixmap(0, onlineIcon);
  					break;
  				}
  				case LUN:
  				{
  					tmpcontact->setPixmap(0, onlineIcon);
  					break;
  				}
 			}
 	 		if( engine->isBlocked( userid ) )
 	 		{
 	 			tmpcontact->setText(0,  publicname + i18n(" Blocked") );
 	 			tmpcontact->setPixmap(0, onlineIcon);
 	 		}

 	 	}
 	}
}

void MSNProtocol::slotDisconnected()
{
		statusBarIcon->setPixmap(&offlineIcon);
}

void MSNProtocol::slotConnectedToMSN(bool c)
{
		connected = c;
		if (c)
		{
			slotConnected();
		}
		else
		{
			slotDisconnected();
		}
}

void MSNProtocol::slotUserStateChange (QString st1, QString st2, int i3)
{
	kdDebug() << "MSN Plugin: User State change " << st1 << " " << st2 << " " << i3 <<"\n";
}

void MSNProtocol::slotInitContacts (QString status, QString userid, QString nick)
{
	kdDebug() << "MSN Plugin: User State change " << status << " " << userid << " " << nick <<"\n";
	if ( status == "NLN" )
	{
		MSNContact *newContact = new MSNContact(userid, nick, this);
		newContact->setPixmap(0,onlineIcon);
	}
}


void MSNProtocol::slotUserSetOffline (QString str)
{
	kdDebug() << "MSN Plugin: User Set Offline " << str << "\n";
		
}
// Dont use this for now
void MSNProtocol::slotNewUserFound (QString userid )
{
	QString tmpnick = engine->getPublicName(userid);
	kdDebug() << "MSN Plugin: User found " << userid << " " << tmpnick <<"\n";
	MSNContact *newContact = new MSNContact(userid, tmpnick, this);
	newContact->setPixmap(0,offlineIcon);		

}
// Dont use this for now
void MSNProtocol::slotNewUser (QString userid )
{
	QString tmpnick = engine->getPublicName(userid);
	kdDebug() << "MSN Plugin: User found " << userid << " " << tmpnick <<"\n";
	MSNContact *newContact = new MSNContact(userid, tmpnick, this);
	newContact->setPixmap(0,offlineIcon);		

}

void MSNProtocol::slotAuthenticate( QString handle )
{
	NewUserImpl *authDlg = new NewUserImpl(0);
	authDlg->setHandle(handle);
	connect( authDlg, SIGNAL(addUser( QString )), this, SLOT(slotAddContact( QString )));
	connect( authDlg, SIGNAL(blockUser( QString )), this, SLOT(slotBlockContact( QString )));
	authDlg->show();
}

void MSNProtocol::slotAddContact( QString handle )
{
	engine->contactAdd( handle );
}

void MSNProtocol::slotBlockContact( QString handle)
{
	engine->contactBlock( handle );
}		
		