/*
    videodevicepool.h  -  Kopete Multiple Video Device handler Class

    Copyright (c) 2005-2006 by Cláudio da Silveira Pinheiro   <taupter@gmail.com>

    Kopete    (c) 2002-2003      by the Kopete developers  <kopete-devel@kde.org>

    *************************************************************************
    *                                                                       *
    * This library is free software; you can redistribute it and/or         *
    * modify it under the terms of the GNU Lesser General Public            *
    * License as published by the Free Software Foundation; either          *
    * version 2 of the License, or (at your option) any later version.      *
    *                                                                       *
    *************************************************************************
*/

#include "videodevicemodelpool.h"

namespace Kopete {

namespace AV {

VideoDeviceModelPool::VideoDeviceModelPool()
{
}


VideoDeviceModelPool::~VideoDeviceModelPool()
{
}

void VideoDeviceModelPool::clear()
{
	m_devicemodel.clear();
}

size_t VideoDeviceModelPool::size()
{
	return m_devicemodel.size();
}

size_t VideoDeviceModelPool::addModel( QString newmodel )
{
	if(m_devicemodel.size())
	{
		for (int loop = 0; loop < m_devicemodel.size(); loop++)
		if (newmodel == m_devicemodel[loop].model)
		{
			kDebug() << "Model " << newmodel << " already exists.";
			m_devicemodel[loop].count++;
			return m_devicemodel[loop].count;
		}
	}
	VideoDeviceModel newdevicemodel;
	newdevicemodel.model=newmodel;
	newdevicemodel.count=0;
	m_devicemodel.push_back(newdevicemodel);
	return 0;
}

void VideoDeviceModelPool::removeModel( QString model )
{
	if(m_devicemodel.size())
	{
		for (int loop = 0; loop < m_devicemodel.size(); loop++)
		if (model == m_devicemodel[loop].model)
		{
			kDebug() << "Model " << model << " removed.";
			if (m_devicemodel[loop].count > 0)
				m_devicemodel[loop].count--;
			else
				m_devicemodel.remove(loop);
		}
	}
}

}

}