#pragma once

#include <string>
#include "GameEvent.h"

using namespace std;

class MessageEvent : public GameEvent
{
public:
	MessageEvent(const string& theString);
	~MessageEvent();

	inline string getString() const { return mString; };

private:
	string mString;
};