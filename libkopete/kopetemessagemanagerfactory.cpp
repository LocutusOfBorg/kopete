
#include "kopetemessagemanagerfactory.h"
#include <kdebug.h>

KopeteMessageManagerFactory::KopeteMessageManagerFactory()
{
}

KopeteMessageManagerFactory::~KopeteMessageManagerFactory()
{
}

KopeteMessageManager *KopeteMessageManagerFactory::create( const KopeteContact *user,KopeteContactList &contacts ,QString logFile = QString::null )
{

	bool createNewSession = false;
	KopeteMessageManager *tmp;
	for ( tmp = mSessionList.first(); tmp ; tmp = mSessionList.next() )
	{
    	if ( user == tmp->user() )
		{
			kdDebug() << "[KopeteMessageManagerFactory] User match, looking session members" << endl;	
			KopeteContact *tmp_contact;
			KopeteContactList contactlist = tmp->contacts();
            for (  tmp_contact = contactlist.first(); tmp ; tmp_contact = contactlist.next() )
			{
				if ( !contacts.containsRef( tmp_contact ) )
				{
					kdDebug() << "[KopeteMessageManagerFactory] create() Oops, contact not found! new session needed!" << endl;	
					createNewSession = true;
				}
			}
			if ( createNewSession == false )
			{
				/* current session (tmp) is the same session the user is requesting */
				return tmp;
			}		
		}
		else
		{
			kdDebug() << "[KopeteMessageManagerFactory] User doesnt match, trying next session" << endl;	
		}
	}
	KopeteMessageManager *session = new KopeteMessageManager ( user, contacts , logFile);
	connect( session, SIGNAL(dying(KopeteMessageManager*)), this, SLOT(slotRemoveSession(KopeteMessageManager*)));
	(mSessionList).append(session);
	return (session);

}

void KopeteMessageManagerFactory::slotRemoveSession( KopeteMessageManager *session)
{
	mSessionList.setAutoDelete(false);
	(mSessionList).remove(session);
}