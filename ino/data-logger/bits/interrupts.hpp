#ifndef BITS_INTERRUPTS_H
#define BITS_INTERRUPTS_H

#include "types.hpp"

namespace Bits {
	using Interrupt = void();

	enum class InterruptType {
		BIT_COMPARE_A,
		BIT_COMPARE_B,
		BIT_OVERFLOW,
		BIT_CAPTURE,
	};

	namespace {
		static ref<Interrupt> iCompareA = nullptr, iCompareB = nullptr, iOverflow = nullptr, iCapture = nullptr;

		ISR(TIM1_COMPA_vect)	{if (iCompareA)	iCompareA();	}
		ISR(TIM1_COMPB_vect)	{if (iCompareB)	iCompareB();	}
		ISR(TIM1_OVF_vect)		{if (iOverflow)	iOverflow();	}
		ISR(TIM1_CAPT_vect)		{if (iCapture)	iCapture();		}
	}

	inline void setTimer1Interrupt(InterruptType const type, ref<Interrupt> const interrupt) {
		switch (type) {
			case InterruptType::BIT_COMPARE_A:	iCompareA	= interrupt; break;
			case InterruptType::BIT_COMPARE_B:	iCompareB	= interrupt; break;
			case InterruptType::BIT_OVERFLOW:	iOverflow	= interrupt; break;
			case InterruptType::BIT_CAPTURE:	iCapture	= interrupt; break;
		}
	}
}

#endif