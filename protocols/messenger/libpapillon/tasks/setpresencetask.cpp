/*
   setpresencetask.h - Set our own presence on Windows Live Messenger service.

   Copyright (c) 2006 by Michaël Larouche <larouche@kde.org>

   *************************************************************************
   *                                                                       *
   * This library is free software; you can redistribute it and/or         *
   * modify it under the terms of the GNU Lesser General Public            *
   * License as published by the Free Software Foundation; either          *
   * version 2 of the License, or (at your option) any later version.      *
   *                                                                       *
   *************************************************************************
*/
#include "Papillon/Tasks/SetPresenceTask"

// Qt includes
#include <QtCore/QStringList>
#include <QtCore/QLatin1String>
#include <QtDebug>

// Papillon includes
#include "Papillon/Client"
#include "Papillon/Connection"
#include "Papillon/NetworkMessage"
#include "Papillon/Global"

namespace Papillon
{

class SetPresenceTask::Private
{
public:
	Private()
	 : features(0)
	{}

	Papillon::Presence::Status presence;
	Papillon::ClientInfo::Features features;

	// Keep track of the expected transaction ID.
	QString currentTransactionId;
};

SetPresenceTask::SetPresenceTask(Papillon::Task *parent)
 : Papillon::Task(parent), d(new Private)
{
}

SetPresenceTask::~SetPresenceTask()
{
	delete d;
}

void SetPresenceTask::setPresence(Papillon::Presence::Status presence)
{
	d->presence = presence;
}

void SetPresenceTask::setClientFeatures(Papillon::ClientInfo::Features features)
{
	d->features = features;
}

bool SetPresenceTask::take(NetworkMessage *networkMessage)
{
	if( networkMessage->command() == QLatin1String("CHG") && networkMessage->transactionId() == d->currentTransactionId )
	{
		// End this task
		setSuccess();
		return true;
	}

	return false;
}

void SetPresenceTask::onGo()
{
	d->currentTransactionId = QString::number( connection()->transactionId() );

	NetworkMessage *setPresenceMessage = new NetworkMessage(NetworkMessage::TransactionMessage);
	setPresenceMessage->setCommand( QLatin1String("CHG") );
	setPresenceMessage->setTransactionId( d->currentTransactionId );
	
	// Set arguments
	QStringList args;
	// String representation of the presence
	args << Papillon::Global::presenceToString(d->presence);
	// Features that we support
	args << QString::number( static_cast<int>(d->features) );
	// TODO: Add MSNObject

	setPresenceMessage->setArguments( args );

	qDebug() << Q_FUNC_INFO << "Changing our own presence to:" << Papillon::Global::presenceToString(d->presence);

	send(setPresenceMessage);
}

}

#include "setpresencetask.moc"