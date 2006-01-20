/*
    Kopete Yahoo Protocol
    Change our Status

    Copyright (c) 2005 André Duffeck <andre.duffeck@kdemail.net>

    *************************************************************************
    *                                                                       *
    * This library is free software; you can redistribute it and/or         *
    * modify it under the terms of the GNU Lesser General Public            *
    * License as published by the Free Software Foundation; either          *
    * version 2 of the License, or (at your option) any later version.      *
    *                                                                       *
    *************************************************************************
*/

#include "changestatustask.h"
#include "transfer.h"
#include "ymsgtransfer.h"
#include "yahootypes.h"
#include "client.h"
#include <qstring.h>

ChangeStatusTask::ChangeStatusTask(Task* parent) : Task(parent)
{
	kdDebug(YAHOO_RAW_DEBUG) << k_funcinfo << endl;
}

ChangeStatusTask::~ChangeStatusTask()
{
}

void ChangeStatusTask::onGo()
{
	kdDebug(YAHOO_RAW_DEBUG) << k_funcinfo << endl;

	YMSGTransfer *t = new YMSGTransfer( Yahoo::ServiceStatus );
	t->setId( client()->sessionID() );

	if( m_status == Yahoo::StatusInvisible )			// status --> Invisible
	{
		sendVisibility( Invisible );
	}
	else
	{
		if( !m_message.isEmpty() )
			m_status = Yahoo::StatusCustom;
	
		if( m_status == Yahoo::StatusCustom )
			t->setParam( 19, m_message.utf8() );
		t->setParam( 10, m_status );
		t->setParam( 47, m_type );
			
		send( t );

		if( client()->status() == Yahoo::StatusInvisible )	// Invisible --> Status
			sendVisibility( Visible );
	}
	setSuccess( true );
}

void ChangeStatusTask::sendVisibility( Visibility visible )
{
	YMSGTransfer *t = new YMSGTransfer( Yahoo::ServiceVisibility );
	t->setId( client()->sessionID() );
	t->setParam( 13, visible );
	send( t );
}

void ChangeStatusTask::setMessage( const QString &msg )
{
	m_message = msg;
}

void ChangeStatusTask::setStatus( Yahoo::Status status )
{
	m_status = status;
}

void ChangeStatusTask::setType( Yahoo::StatusType type )
{
	m_type = type;
}
