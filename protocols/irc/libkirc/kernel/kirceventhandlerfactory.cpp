/*
    kirceventhandlerfactory.cpp - IRC event handler factory.

    Copyright (c) 2008      by Michel Hermier <michel.hermier@gmail.com>

    *************************************************************************
    *                                                                       *
    * This program is free software; you can redistribute it and/or modify  *
    * it under the terms of the GNU General Public License as published by  *
    * the Free Software Foundation; either version 2 of the License, or     *
    * (at your option) any later version.                                   *
    *                                                                       *
    *************************************************************************
*/

#include "kirceventhandlerfactory.h"

using namespace KIrc;

QStringList EventHandlerFactory::keys()
{
	return QStringList();
}

KIrc::EventHandler *EventHandlerFactory::create(const QString &key, QObject *parent)
{
	return 0;
}

