#include "EventSystem.h"
#include "Event.h"
#include "EventListener.h"
#include <cassert>

EventSystem* EventSystem::mspInstance = nullptr;

EventSystem::EventSystem()
{
}

EventSystem::~EventSystem()
{
	cleanup();
}

void EventSystem::addListener(EventType type, EventListener* pListener)
{
	if (mIsInitted)
	{
		mListenerMap.insert(std::pair< EventType, EventListener* >(type, pListener));
	}
	else
	{
		cout << "EventSystem::init not called - call init prior to using!\n";
		assert(false);
	}
}

void EventSystem::removeListener(EventType type, EventListener *pListener)
{
	if (mIsInitted)
	{
		std::pair<std::multimap<EventType, EventListener*>::iterator, std::multimap<EventType, EventListener*>::iterator> ret;

		ret = mListenerMap.equal_range(type);
		std::multimap<EventType, EventListener*>::iterator iter;

		for (iter = ret.first; iter != ret.second; ++iter)
		{
			if (iter->second == pListener)
			{
				mListenerMap.erase(iter);
				break;//to prevent using invalidated iterator
			}
		}
	}
	else
	{
		cout << "EventSystem::init not called - call init prior to using!\n";
		assert(false);
	}

}

void EventSystem::removeListenerFromAllEvents( EventListener* pListener )
{
	if (mIsInitted)
	{
		std::multimap<EventType, EventListener*>::iterator iter;

		bool allTheWayThrough = false;

		while (!allTheWayThrough)
		{
			allTheWayThrough = true;
			for (iter = mListenerMap.begin(); iter != mListenerMap.end(); ++iter)
			{
				if (iter->second == pListener)
				{
					mListenerMap.erase(iter);
					allTheWayThrough = false; //didn't make it the whole way through
					break;//to prevent using invalidated iterator
				}
			}
		}
	}
	else
	{
		cout << "EventSystem::init not called - call init prior to using!\n";
		assert(false);
	}
}

EventSystem* EventSystem::getInstance()
{
	assert(mspInstance);
	return mspInstance;
}

void EventSystem::createInstance()
{
	assert(!mspInstance);
	mspInstance = new EventSystem;
}

void EventSystem::destroyInstance()
{
	delete mspInstance;
	mspInstance = nullptr;
}

void EventSystem::init()
{
	if (mIsInitted)
	{
		cleanup();
	}
	mIsInitted = true;
}

void EventSystem::cleanup()
{
	mListenerMap.clear();
	mIsInitted = false;
}

void EventSystem::fireEvent( const Event& theEvent )
{
	if (mIsInitted)
	{
		dispatchAllEvents(theEvent);
	}
	else
	{
		cout << "EventSystem::init not called - call init prior to using!\n";
		assert(false);
	}
}

void EventSystem::dispatchAllEvents( const Event& theEvent )
{
	if (mIsInitted)
	{
		std::pair<std::multimap<EventType, EventListener*>::iterator, std::multimap<EventType, EventListener*>::iterator> ret;
		ret = mListenerMap.equal_range(theEvent.getType());

		std::multimap<EventType, EventListener*>::iterator iter;
		for (iter = ret.first; iter != ret.second; ++iter)
		{
			iter->second->handleEvent(theEvent);
		}
	}
	else
	{
		cout << "EventSystem::init not called - call init prior to using!\n";
		assert(false);
	}
}