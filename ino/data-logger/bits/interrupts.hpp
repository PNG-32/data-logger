#ifndef BITS_INTERRUPTS_H
#define BITS_INTERRUPTS_H

namespace Bits {
	namespace {
		static interrupt iCompareA = nullptr, iCompareB = nullptr, iOverflow = nullptr, iCapture = nullptr;

		ISR(TIM0_COMPA_vect)	{if (iCompareA)	iCompareA();	}
		ISR(TIM0_COMPB_vect)	{if (iCompareB)	iCompareB();	}
		ISR(TIM0_OVF_vect)		{if (iOverflow)	iOverflow();	}
		ISR(TIM0_CAPT_vect)		{if (iCapture)	iCapture();		}
	}
}

#endif