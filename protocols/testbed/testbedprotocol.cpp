/*
    testbedprotocol.cpp - Kopete Testbed Protocol

    Copyright (c) 2003      by Will Stephenson		 <will@stevello.free-online.co.u>
    Kopete    (c) 2002-2003 by the Kopete developers <kopete-devel@kde.org>

    *************************************************************************
    *                                                                       *
    * This library is free software; you can redistribute it and/or         *
    * modify it under the terms of the GNU Lesser General Public            *
    * License as published by the Free Software Foundation; either          *
    * version 2 of the License, or (at your option) any later version.      *
    *                                                                       *
    *************************************************************************
*/
#include <kgenericfactory.h>
#include <kdebug.h>

#include "kopeteaccountmanager.h"

#include "testbedaccount.h"
#include "testbedcontact.h"
#include "testbedprotocol.h"
#include "testbedaddcontactpage.h"
#include "testbededitaccountwidget.h"

typedef KGenericFactory<TestbedProtocol> TestbedProtocolFactory;
K_EXPORT_COMPONENT_FACTORY( kopete_testbed, TestbedProtocolFactory( "kopete_testbed" )  )

TestbedProtocol *TestbedProtocol::s_protocol = 0L;

TestbedProtocol::TestbedProtocol( QObject* parent, const char *name, const QStringList &/*args*/ )
	: KopeteProtocol( TestbedProtocolFactory::instance(), parent, name ),
	  testbedOnline(  KopeteOnlineStatus::Online, 25, this, 0,  QString::null,  i18n( "Go O&nline" ),   i18n( "Online" ) ),
	  testbedAway(  KopeteOnlineStatus::Away, 25, this, 1, "msn_away",  i18n( "Go &Away" ),   i18n( "Away" ) ),
	  testbedOffline(  KopeteOnlineStatus::Offline, 25, this, 2,  QString::null,  i18n( "Go O&ffline" ),   i18n( "Offline" ) )
	  
{
	kdDebug( 14210 ) << k_funcinfo << endl;

	s_protocol = this;
}

TestbedProtocol::~TestbedProtocol()
{
}

void TestbedProtocol::deserializeContact( KopeteMetaContact *metaContact, const QMap<QString, QString> &serializedData,
	const QMap<QString, QString> &/* addressBookData */)
{
	QString contactId = serializedData[ "contactId" ];
	QString accountId = serializedData[ "accountId" ];
	QString displayName = serializedData[ "displayName" ];
	QString type = serializedData[ "contactType" ];
	
	TestbedContact::TestbedContactType tbcType;
	if ( type == QString::fromLatin1( "echo" ) )
		tbcType = TestbedContact::Echo;
	if ( type == QString::fromLatin1( "null" ) )
		tbcType = TestbedContact::Null;
	else
		tbcType = TestbedContact::Null;
		
	QDict<KopeteAccount> accounts = KopeteAccountManager::manager()->accounts( this );

	KopeteAccount *account = accounts[ accountId ];
	if ( !account )
	{
		kdDebug(14210) << "Account doesn't exist, skipping" << endl;
		return;
	}

	new TestbedContact(account, contactId, tbcType, displayName, metaContact);
}

AddContactPage * TestbedProtocol::createAddContactWidget( QWidget *parent, KopeteAccount * /* account */ )
{
	kdDebug( 14210 ) << "Creating Add Contact Page" << endl;
	return new TestbedAddContactPage( parent );
}

EditAccountWidget * TestbedProtocol::createEditAccountWidget( KopeteAccount *account, QWidget *parent )
{
	kdDebug(14210) << "Creating Edit Account Page" << endl;
	return new TestbedEditAccountWidget( parent, account );
}

KopeteAccount *TestbedProtocol::createNewAccount( const QString &accountId )
{
	return new TestbedAccount( this, accountId );
}

TestbedProtocol *TestbedProtocol::protocol()
{
	return s_protocol; 
}



#include "testbedprotocol.moc"
