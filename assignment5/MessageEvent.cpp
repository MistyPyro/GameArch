#include "MessageEvent.h"

MessageEvent::MessageEvent(const string& theString)
	:GameEvent(MESSAGE_EVENT)
	, mString(theString)
{
}

MessageEvent::~MessageEvent()
{
}