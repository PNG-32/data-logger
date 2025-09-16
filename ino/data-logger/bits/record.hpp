#ifndef BITS_RECORD_H
#define BITS_RECORD_H

#include "types.hpp"

namespace Bits {
	enum class CacheType: uint8 {
		BCT_NO_CACHE,
		BCT_CACHE_SAVE_ON_SET,
		BCT_CACHE_MANUAL_SAVE,
		BCT_CACHE_SAVE_ON_DTOR
	};

	template <class T, CacheType CACHE = CacheType::BCT_NO_CACHE, bool AUTOINIT = false>
	struct Record;

	template <class T, bool AUTOINIT>
	struct Record<T, CacheType::BCT_NO_CACHE, AUTOINIT> {
		Record(uint16 const address):
			memaddr(address) {}

		void begin() {}

		T get() const {
			T val;
			fetch(val);
			return val;
		}

		void fetch(T& out) const	{EEPROM.get(memaddr, out);					}

		Record& set(T const& val)	{EEPROM.put(memaddr, val); return *this;	}

		void save()					{}

		inline operator T() const				{return get();		}
		inline Record& operator=(T const& val)	{return set(val);	}

		constexpr uint16 address() const {return memaddr;}

	private:
		uint16 const memaddr;
	};

	template <class T, bool AUTOINIT>
	struct Record<T, CacheType::BCT_CACHE_SAVE_ON_SET, AUTOINIT> {
		Record(uint16 const address):
			memaddr(address) {if (AUTOINIT) begin();}

		void begin() {EEPROM.get(memaddr, val);}

		T get() const {
			return val;
		}

		void fetch(T& out) const {
			out = val;
		}

		Record& set(T const& v) {
			val = v;
			save();
			return *this;
		}

		void save() {EEPROM.put(memaddr, val);}

		inline operator T() const				{return get();		}
		inline Record& operator=(T const& val)	{return set(val);	}

		constexpr uint16 address() const {return memaddr;}

	private:
		uint16 const memaddr;
		T val;
	};

	template <class T, bool AUTOINIT>
	struct Record<T, CacheType::BCT_CACHE_MANUAL_SAVE, AUTOINIT> {
		Record(uint16 const address):
			memaddr(address) {if (AUTOINIT) begin();}

		void begin() {EEPROM.get(memaddr, val);}

		T get() const {
			return val;
		}

		void fetch(T& out) const {
			out = val;
		}

		Record& set(T const& v) {
			val = v;
			 return *this;
		}

		void save() {EEPROM.put(memaddr, val);}

		inline operator T() const				{return get();		}
		inline Record& operator=(T const& val)	{return set(val);	}

		constexpr uint16 address() const {return memaddr;}

	private:
		uint16 const memaddr;
		T val;
	};

	template <class T, bool AUTOINIT>
	struct Record<T, CacheType::BCT_CACHE_SAVE_ON_DTOR, AUTOINIT> {
		Record(uint16 const address):
			memaddr(address) {if (AUTOINIT) begin();}

		~Record() {save();}

		void begin() {EEPROM.get(memaddr, val);}

		T get() const {
			return val;
		}

		void fetch(T& out) const {
			out = val;
		}

		Record& set(T const& v) {
			val = v;
			return *this;
		}

		void save() {EEPROM.put(memaddr, val);}

		inline operator T() const				{return get();		}
		inline Record& operator=(T const& val)	{return set(val);	}

		constexpr uint16 address() const {return memaddr;}

	private:
		uint16 const memaddr;
		T val;
	};
}

#endif
