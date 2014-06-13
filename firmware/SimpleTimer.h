


#ifndef __TELNET_SIMPLETIMER__
#define __TELNET_SIMPLETIMER__

#include <spark_wiring.h>

namespace Telnet {

class SimpleTimer {

private:
	long mStart;

public:
	SimpleTimer() : mStart(0) {}

	void start() {
		mStart = millis();
	}

	long elapsed() {
		return millis() - mStart;
	}

};

};


#endif
