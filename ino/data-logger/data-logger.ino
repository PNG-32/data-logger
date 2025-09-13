#include "bits/bits.hpp"

Bits::DataLogger logger;

void setup()	{logger.begin();	}
void loop()		{logger.update();	}