/*
    kopetemetacontact.cpp - Kopete Meta Contact

    Copyright (c) 2002 by Martijn Klingens       <klingens@kde.org>
    Copyright (c) 2002 by Duncan Mac-Vicar Prett <duncan@kde.org>

    Kopete    (c) 2002 by the Kopete developers  <kopete-devel@kde.org>

    *************************************************************************
    *                                                                       *
    * This program is free software; you can redistribute it and/or modify  *
    * it under the terms of the GNU General Public License as published by  *
    * the Free Software Foundation; either version 2 of the License, or     *
    * (at your option) any later version.                                   *
    *                                                                       *
    *************************************************************************
*/

#include "kopetemetacontact.h"

#include <qapplication.h>
#include <qdom.h>
#include <qptrlist.h>
#include <qstylesheet.h>
#include <qregexp.h>

#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <knotifyclient.h>

#include "kopetecontactlist.h"
#include "kopeteplugin.h"
#include "kopeteprotocol.h"
#include "kopeteprefs.h"
#include "pluginloader.h"

#if KDE_VERSION >= 306
#include <kpassivepopup.h>
#include "systemtray.h"
#endif

KopeteMetaContact::KopeteMetaContact()
: QObject( KopeteContactList::contactList() )
{
	//TODO: implement m_trackChildNameChanges
	m_trackChildNameChanges = true;
	m_temporary=false;
//	m_isTopLevel=false;

	m_onlineStatus = Unknown;
	m_idleState = Unspecified;
}

KopeteMetaContact::~KopeteMetaContact()
{
}

void KopeteMetaContact::addContact( KopeteContact *c )
{
	if( m_contacts.contains( c ) )
	{
		kdWarning(14010) << "Ignoring attempt to add duplicate contact " << c->contactId() << "!" << endl;
	}
	else
	{
		m_contacts.append( c );

		connect( c, SIGNAL( statusChanged( KopeteContact *,
			KopeteContact::ContactStatus ) ),
			this, SLOT( slotContactStatusChanged( KopeteContact *,
			KopeteContact::ContactStatus ) ) );

		connect( c, SIGNAL( displayNameChanged( const QString & ) ),
			this, SLOT( slotContactNameChanged( const QString & ) ) );

		connect( c, SIGNAL( contactDestroyed( KopeteContact * ) ),
			this, SLOT( slotContactDestroyed( KopeteContact * ) ) );

		connect( c, SIGNAL( idleStateChanged( KopeteContact *, KopeteContact::IdleState ) ),
			this, SLOT( slotContactIdleStateChanged( KopeteContact *, KopeteContact::IdleState ) ) );

		if (m_displayName == QString::null)
		{
			 setDisplayName( c->displayName() );
			 m_trackChildNameChanges=true;
		}

	/*	for( QStringList::ConstIterator it = groups.begin(); it != groups.end(); ++it )
		{
			addToGroup(*it);
		}*/
		emit contactAdded(c);
	}

	updateOnlineStatus();
}

void KopeteMetaContact::updateOnlineStatus()
{
	OnlineStatus newStatus = Unknown;

	QPtrListIterator<KopeteContact> it( m_contacts );
	for( ; it.current(); ++it )
	{
		KopeteContact::ContactStatus s = it.current()->status();

		if ( s == KopeteContact::Online )
		{
			newStatus = Online;
			break;
		}
		else if ( s == KopeteContact::Away )
		{
			// Set status, but don't stop searching, since 'Online' overrules
			// 'Away'
			newStatus = Away;
		}
		else if (s == KopeteContact::Offline && newStatus!=Away)
		{
			newStatus = Offline;
		}
	}

	if( newStatus != m_onlineStatus )
	{
		m_onlineStatus = newStatus;
		emit onlineStatusChanged( this, m_onlineStatus );
	}
}

void KopeteMetaContact::updateIdleState()
{
	IdleState newStatus = Unspecified;

	QPtrListIterator<KopeteContact> it( m_contacts );
	for( ; it.current(); ++it )
	{
		KopeteContact::IdleState s = it.current()->idleState();

		if ( s == KopeteContact::Active )
		{
			newStatus = Active;
			break;
		}
		else if ( s == KopeteContact::Idle )
		{
			// Set status, but don't stop searching, since 'Active' overrules
			// 'Idle'
			newStatus = Idle;
		}
	}

	if( newStatus != m_idleState )
	{
		m_idleState = newStatus;
		emit idleStateChanged( this, m_idleState );
	}
}

void KopeteMetaContact::removeContact(KopeteContact *c, bool deleted)
{
	if( !m_contacts.contains( c ) )
	{
		kdDebug(14010) << "KopeteMetaContact::removeContact: Contact is not in this metaContact " << endl;
	}
	else
	{
		m_contacts.remove( c );

		if(!deleted)
		{  //If this function is tell by slotContactRemoved, c is maybe just a QObject
			disconnect( c, SIGNAL( statusChanged( KopeteContact *,
				KopeteContact::ContactStatus ) ),
				this, SLOT( slotContactStatusChanged( KopeteContact *,
				KopeteContact::ContactStatus ) ) );

			disconnect( c, SIGNAL( displayNameChanged( const QString & ) ),
				this, SLOT( slotContactNameChanged( const QString & ) ) );

			disconnect( c, SIGNAL( contactDestroyed( KopeteContact * ) ),
				this, SLOT( slotContactDestroyed( KopeteContact * ) ) );

			disconnect( c, SIGNAL( idleStateChanged( KopeteContact *, KopeteContact::IdleState ) ),
				this, SLOT( slotContactIdleStateChanged( KopeteContact *, KopeteContact::IdleState ) ) );

			kdDebug(14010) << "KopeteMetaContact::removeContact: Contact disconected" << endl;
		}
		emit contactRemoved(c);
	}
	updateOnlineStatus();
}


bool KopeteMetaContact::isTopLevel()
{
	if(m_groups.isEmpty())
		m_groups.append(KopeteGroup::toplevel);
	return (m_groups.contains(KopeteGroup::toplevel));
}

void KopeteMetaContact::setTopLevel( bool b )
{
	if(b)
	{
		if(!isTopLevel())
			m_groups.append(KopeteGroup::toplevel);
	}
	else
	{
			m_groups.remove(KopeteGroup::toplevel);
	}
}

KopeteContact *KopeteMetaContact::findContact( const QString &protocolId,
	const QString &identityId, const QString &contactId )
{
	//kdDebug(14010) << "*** Num contacts: " << m_contacts.count() << endl;
	QPtrListIterator<KopeteContact> it( m_contacts );
	for( ; it.current(); ++it )
	{
		//kdDebug(14010) << "*** Trying " << it.current()->contactId() << ", proto " << it.current()->protocol()->pluginId() << ", identity " << it.current()->identityId() << endl;
		if( (it.current()->contactId() == contactId ) && (it.current()->protocol()->pluginId() == protocolId ) && (it.current()->identityId() == identityId))
			return it.current();
	}

	// Contact not found
	return 0L;
}

void KopeteMetaContact::sendMessage()
{
	kdDebug(14010) << "KopeteMetaContact::sendMessage() not implemented!" << endl;

	/*
		Algorithm:
		1. Determine the protocol to use
		   a. In the configuration the user can specify the order in which
		      protocols are tried, so if multiple protocols are available, the
		      preferred one is used.
		   b. Iterate over the preference order to see if a contact exists
		      and is online for this protocol and if the same protocol is
		      connected. Offline contacts are ignored, as are contacts for
		      which the protocol is currently disconnected.
		   c. If the protocol is not found in step b, repeat, but now looking
		      for protocols that support offline messages and send an offline
		      message.
		   d. If no combination of a connected protocol and a reachable user
		      ( either with online or offline messages ) is found, display
		      an error and return.
		2. Show dialog for entering the message
		3. In the background, try to open a chat session. This is a no-op
		   for message-based protocols like ICQ
		4. Send the message
		5. Close the chat session, if any, for the connection-based protocols
	*/
}

void KopeteMetaContact::startChat()
{
	kdDebug(14010) << "KopeteMetaContact::startChat() not implemented!" << endl;

	/*
		Algorithm:
		1. Determine the protocol as described in sendMessage(), with a
		   small difference:
		   If a contact can *only* be reached by using offline messages,
		   notify the user and switch to sendMessage() mode. Interactive
		   chats with offline users are a bit pointless after all...
		2. Show the chat dialog
		3. Open the session in the background
		4. Send messages, until the dialog is closed
		5. Close the chat session

		Caveats:
		- while connecting in protocols like MSN, messages might get 'sent'
		  by the user before the connection is open. Until the connection
		  signals a 'ready' condition all messages should be queued. This
		  should be done here to avoid code duplication
		- while sending messages you have to wait for the server confirmation
		  with most protocols. In MSN this latency is small enough to not
		  notice and MSN also requires a builtin queue for protocol-specific
		  reasons, but in general the queuing should be done here. Until the
		  chat session signals a 'message sent' condition any subsequent
		  messages should be put on hold and queued. Kopete should also show
		  a simple spinning hour glass animation or something similar to
		  indicate that messages are still pending.
	*/

	// FIXME: Implement the above!
	//now just select the highter status importance
	if( m_contacts.isEmpty() )
		return;

	KopeteContact *c = 0L;
	for( QPtrListIterator<KopeteContact> it( m_contacts ) ; it.current(); ++it )
	{
		if( ( !c || ( *it )->importance() > c->importance() ) &&
			( *it )->isReachable() )
		{
			c = *it;
		}
	}

	if( c )
	{
		c->execute();
	}
	else
	{
		KMessageBox::error( qApp->mainWidget(),
			i18n( "This user is not reachable at the moment. Please"
				"try a protocol that supports offline sending, or wait "
				"until this user goes online." ),
			i18n( "User is not reachable - Kopete" ) );
	}
}

void KopeteMetaContact::execute()
{
	// FIXME: Implement, don't hardcode startChat()!
	startChat();
}

QString KopeteMetaContact::statusIcon() const
{
	switch( status() )
	{
		case Online:
			return "metacontact_online";
		case Away:
			return "metacontact_away";
		case Offline:
		case Unknown:
		default:
			return "metacontact_offline";
	}
}

QString KopeteMetaContact::statusString() const
{
	switch( status() )
	{
		case Online:
			return i18n("Online");
		case Away:
			return i18n("Away");
		case Offline:
			return i18n("Offline");
		case Unknown:
		default:
			return i18n("Status not available");
	}
}

KopeteMetaContact::OnlineStatus KopeteMetaContact::status() const
{
	return m_onlineStatus;
}

bool KopeteMetaContact::isOnline() const
{
	QPtrListIterator<KopeteContact> it( m_contacts );
	for( ; it.current(); ++it )
	{
		if( it.current()->isOnline() )
			return true;
	}
	return false;
}

bool KopeteMetaContact::isReachable() const
{
	if( isOnline() )
		return true;

	QPtrListIterator<KopeteContact> it( m_contacts );
	for( ; it.current(); ++it )
	{
		// FIXME: implement KopeteContact::protocol()!!!
		//if( it.current()->protocol()->canSendOffline() )
		//	return true;
		continue;
	}
	return false;
}

//Determine if we are capable of accepting file transfers
bool KopeteMetaContact::canAcceptFiles() const
{
	if( !isOnline() )
		return false;

	QPtrListIterator<KopeteContact> it( m_contacts );
	for( ; it.current(); ++it )
	{
		if( it.current()->canAcceptFiles() )
			return true;
	}
	return false;
}

//Slot for sending files
void KopeteMetaContact::sendFile( const KURL &sourceURL, const QString &altFileName, unsigned long fileSize )
{
	//If we can't send any files then exit
	if( m_contacts.isEmpty() || !canAcceptFiles() )
		return;

	//Find the highest ranked protocol that can accept files
	KopeteContact *c=m_contacts.first();
	for(QPtrListIterator<KopeteContact> it( m_contacts ) ; it.current(); ++it )
	{
		if( ( (*it)->importance() > c->importance() ) && ( (*it)->canAcceptFiles() ) )
			c=*it;
	}

	//Call the sendFile slot of this protocol
	c->sendFile( sourceURL, altFileName, fileSize );
}

void KopeteMetaContact::slotContactStatusChanged( KopeteContact * c,
	KopeteContact::ContactStatus  s  )
{
	emit contactStatusChanged(c,s);
	OnlineStatus m = m_onlineStatus;
	updateOnlineStatus();
	if ( (m_onlineStatus != m) && (m_onlineStatus==Online) && (KopetePrefs::prefs()->soundNotify()) )
	{
		#if KDE_VERSION >= 306
		if ( KopetePrefs::prefs()->notifyOnline() )
		{
			KPassivePopup::message( i18n( "%2 is now %1!" ).arg(
				statusString() ).arg( displayName() ),
				KopeteSystemTray::systemTray() );
		}
		#endif

		KopeteProtocol* p = dynamic_cast<KopeteProtocol*>(c->protocol());
		if (!p)
		{
			kdDebug(14010) <<"KopeteMetaContact::slotContactStatusChanged: KopeteContact is not from a valid Protocol" <<endl;
			return;
		}
		if ( !p->isAway() || KopetePrefs::prefs()->soundIfAway() )
			KNotifyClient::event("kopete_online");
	}
}

void KopeteMetaContact::setDisplayName( const QString &name )
{
	m_displayName = name;
	m_trackChildNameChanges = false;
	emit displayNameChanged( this, name );
}

QString KopeteMetaContact::displayName() const
{
	return m_displayName;
}

void KopeteMetaContact::slotContactNameChanged( const QString &name )
{
	if( m_trackChildNameChanges )
	{
		setDisplayName( name );
		//because m_trackChildNameChanges is set to flase in setDisplayName
		m_trackChildNameChanges = true;
	}
}

void KopeteMetaContact::moveToGroup(  KopeteGroup *from,  KopeteGroup *to )
{
	if(m_temporary && to!=KopeteGroup::temporary)
		return;

	if (!from || !m_groups.contains( from ) || (from==KopeteGroup::toplevel && !isTopLevel()))
	{
		addToGroup(to);
		return;
	}

	if(!to ||  m_groups.contains( to ) || (to==KopeteGroup::toplevel && isTopLevel()) )
	{
		removeFromGroup(from);
		return;
	}

//	kdDebug(14010) << "KopeteMetaContact::moveToGroup: "<<from.string() <<" => "<<to.string() << endl;

	m_groups.remove( from );

	m_groups.append( to );

/*	for( 	KopeteContact *c = m_contacts.first(); c ; c = m_contacts.next() )
	{
		c->moveToGroup(from,to);
	}*/

	emit movedToGroup(from, to, this );
}

void KopeteMetaContact::removeFromGroup(  KopeteGroup *from)
{
	if(m_temporary && from==KopeteGroup::temporary)
		return ;

	if (!from || !m_groups.contains( from ) || (from==KopeteGroup::toplevel && !isTopLevel()))
	{
		/*kdDebug(14010) << "KopeteMetaContact::removeFromGroup: This contact is removed from all groups, deleting contact" <<endl;
		KopeteContactList::contactList()->removeMetaContact(this);*/
		return;
	}

	m_groups.remove( from );


	/*for( 	KopeteContact *c = m_contacts.first(); c ; c = m_contacts.next() )
	{
		c->removeFromGroup(from);
	} */

	emit removedFromGroup(  from, this);
}

void KopeteMetaContact::addToGroup(  KopeteGroup *to )
{
	if(m_temporary && to!=KopeteGroup::temporary)
		return;

	if(!to ||  m_groups.contains( to ) || (to==KopeteGroup::toplevel && isTopLevel()))
	{
		return;
	}

	m_groups.append( to );

	/*for( 	KopeteContact *c = m_contacts.first(); c ; c = m_contacts.next() )
	{
		c->addToGroup(to);
	}*/

	emit addedToGroup( to ,this );
}

KopeteGroupList KopeteMetaContact::groups() const
{
/*	if(m_groups.isEmpty())
		m_groups.append(KopeteGroup::toplevel);*/
	return m_groups;
}

void KopeteMetaContact::slotContactDestroyed( KopeteContact *contact )
{
	removeContact(contact,true);
}

QString KopeteMetaContact::toXML()
{
	emit aboutToSave(this);

	QString xml = "  <meta-contact id=\"TODO: KABC ID\">\n"
		"    <display-name>" +
		QStyleSheet::escape( m_displayName ) +
		"</display-name>\n";

	// Store groups
	if ( !m_groups.isEmpty() )
	{
		xml += "    <groups>\n";
		KopeteGroup *g;;
		for ( g = m_groups.first(); g; g = m_groups.next() )
		{
			QString group_s=g->displayName();
			if(!group_s.isNull())
			{
				if ( !group_s.isEmpty() )
					xml += "      <group>" + QStyleSheet::escape( group_s) + "</group>\n";
				else
				  xml += "      <group>" + QStyleSheet::escape( i18n("Unknown") ) + "</group>\n";
			}
		}

		// The contact is also at top-level
		if ( isTopLevel() )
		{
			xml += "      <top-level/>\n";
		}

		xml += "    </groups>\n";
	}
	else
	{
		/*
		   Rare case to prevent bug, if contact has no groups
		   and it is not at top level it should have been deleted.
		   But we didn't, so we put it in toplevel to prevent a
		   hided contact, also for toplevel contacts saved before
		   we added the <top-level> tag.
		*/
		xml += "    <groups><top-level/></groups>\n";
	}

/* //SERIALIZE IS NOW OBSOLETE
	QPtrList<KopetePlugin> ps = LibraryLoader::pluginLoader()->plugins();
	for( KopetePlugin *p = ps.first() ; p != 0L; p = ps.next() )
	{
		QStringList strList;
		if ( p->serialize( this, strList ) && !strList.empty() )
		{
			for ( QStringList::iterator it = strList.begin(); it != strList.end(); ++it )
			{
				//escape '||' I don't like this but it is needed
				(*it)=(*it).replace(QRegExp("\\\\"),"\\\\").replace(QRegExp("\\|"),"\\|;");
			}
			QString data = QStyleSheet::escape( strList.join( "||" ) );
			xml += "    <plugin-data plugin-id=\"" +
				QStyleSheet::escape( p->id())  + "\">" + data  + "</plugin-data>\n";
		}
	}*/

	// Store address book fields
	AddressBookFields::Iterator addrIt = m_addressBook.begin();
	for( ; addrIt != m_addressBook.end(); ++addrIt )
	{
		xml += "    <address-book-field id=\"" + QStyleSheet::escape(addrIt.key()) + "\">" +
					QStyleSheet::escape(addrIt.data()) + "</address-book-field>\n";
	}

	// Store other plugin data
	QMap<QString, QString>::ConstIterator it;
	for( it = m_pluginData.begin(); it != m_pluginData.end(); ++it )
	{
		xml += "    <plugin-data plugin-id=\"" + QStyleSheet::escape(it.key()) + "\">"
				+ QStyleSheet::escape(it.data()) + "</plugin-data>\n";
	}

	xml += "  </meta-contact>\n";

	return xml;
}

bool KopeteMetaContact::fromXML( const QDomNode& cnode )
{
	QDomNode contactNode = cnode;
	while( !contactNode.isNull() )
	{
		QDomElement contactElement = contactNode.toElement();
		if( !contactElement.isNull() )
		{
			if( contactElement.tagName() == "display-name" )
			{
				if ( contactElement.text().isEmpty() )
					return false;
				m_displayName = contactElement.text();
				m_trackChildNameChanges = false;
			}
			else if( contactElement.tagName() == "groups" )
			{
				QDomNode group = contactElement.firstChild();
				while( !group.isNull() )
				{
					QDomElement groupElement = group.toElement();
					if ( groupElement.tagName() == "group" )
					{
						m_groups.append(KopeteContactList::contactList()->getGroup(groupElement.text()));
					}
					else if ( groupElement.tagName() == "top-level" )
					{
						m_groups.append(KopeteGroup::toplevel);
					}

					group = group.nextSibling();
				}
			}

			else if( contactElement.tagName() == "address-book-field" )
			{
				QString id = contactElement.attribute( "id", QString::null );
				QString val = contactElement.text();
				m_addressBook.insert( id, val );

			}
			else if( contactElement.tagName() == "plugin-data" )
			{
				QString pluginId = contactElement.attribute(
					"plugin-id", QString::null );
				m_pluginData.insert( pluginId, contactElement.text() );
			}

		}
		contactNode = contactNode.nextSibling();
	}

	// If a plugin is loaded, load data cached
	connect( LibraryLoader::pluginLoader(), SIGNAL( pluginLoaded(KopetePlugin*) ),
		this, SLOT( slotPluginLoaded(KopetePlugin*) ) );

	return true;
}

QString KopeteMetaContact::addressBookField( KopetePlugin * p,
	const QString & key ) const
{
	if ( p && p->addressBookFields().contains( key ) ) {
		if ( m_addressBook.contains( key ) ) {
			return m_addressBook[ key ];
		} else
			return QString::null;
	} else
		return QString::null;
}

void KopeteMetaContact::setAddressBookField( KopetePlugin * p ,
	const QString & key, const QString & value )
{
	if ( p && p->addressBookFields().contains( key ) )
		m_addressBook.insert( key, value );
	else
		kdDebug(14010) << "[KopeteMetaContact::setAddressBookField] Sorry, plugin "
			  << p->pluginId() << " doesn't have field "
			  << key << " registered" << endl;
}

KopeteMetaContact::AddressBookFields KopeteMetaContact::addressBookFields() const
{
	return m_addressBook;
}

bool KopeteMetaContact::isTemporary() const
{
	return m_temporary;
}
void KopeteMetaContact::setTemporary( bool isTemporary, KopeteGroup *group )
{
	m_temporary = isTemporary;
	KopeteGroup *temporaryGroup = KopeteGroup::temporary;
	if(m_temporary)
	{
		addToGroup (temporaryGroup);
		KopeteGroup *g;
		for ( g = m_groups.first(); g; g = m_groups.next() )
		{
			if(g != temporaryGroup)
				removeFromGroup(g);
		}
	}
	else
		moveToGroup(temporaryGroup, group);
}

bool KopeteMetaContact::isDirty() const
{
	return m_dirty;
}

void KopeteMetaContact::setDirty( bool b  )
{
	m_dirty = b;
}

void KopeteMetaContact::slotPluginLoaded( KopetePlugin *p )
{
	if( !p )
		return;

	QMap<QString, QString>::ConstIterator it;
	for( it = m_pluginData.begin(); it != m_pluginData.end(); ++it )
	{
		if( p->pluginId() == it.key() )
		{
			p->deserialize( this, pluginData(p) );
		}
	}
}

void KopeteMetaContact::setPluginData(KopetePlugin *p, QStringList strList )
{
	if(strList.isEmpty())
	{
		m_pluginData.remove(p->pluginId());
		return;
	}

	for ( QStringList::iterator it = strList.begin(); it != strList.end(); ++it )
	{
		//escape '||' I don't like this but it is needed
		(*it)=(*it).replace(QRegExp("\\\\"),"\\\\").replace(QRegExp("\\|"),"\\|;");
	}
	m_pluginData[p->pluginId()] =  strList.join( "||" ) ;
}

QStringList KopeteMetaContact::pluginData(KopetePlugin *p)
{
	if(!m_pluginData.contains(p->pluginId()))
		return QStringList();

	QStringList strList = QStringList::split( "||", m_pluginData[p->pluginId()] );
	for ( QStringList::iterator it2 = strList.begin(); it2 != strList.end(); ++it2 )
	{
		//unescape '||'
		(*it2)=(*it2).replace(QRegExp("\\\\\\|;"),"|").replace(QRegExp("\\\\\\\\"),"\\");
	}
	return strList;
}

KopeteMetaContact::IdleState KopeteMetaContact::idleState() const
{
	return m_idleState;
}

void KopeteMetaContact::slotContactIdleStateChanged( KopeteContact *c,	KopeteContact::IdleState s )
{
	emit contactIdleStateChanged(c,s);
	updateIdleState();
}

#include "kopetemetacontact.moc"

// vim: set noet ts=4 sts=4 sw=4:


