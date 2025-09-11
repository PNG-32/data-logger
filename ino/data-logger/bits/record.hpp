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

	template <class T, CacheType CACHE = CacheType::BCT_CACHE_SAVE_ON_SET>
	struct Record;

	template <class T>
	struct Record<T, CacheType::BCT_NO_CACHE> {
		Record(uint16 const address):
			memaddr(address) {}

		T get() const {
			T val;
			fetch(val);
			return val;
		}

		void fetch(T& out) const	{EEPROM.get(memaddr, out);}

		void set(T const& val)		{EEPROM.put(memaddr, val);}

		void save()					{}

		inline operator T() const				{return get();		}
		inline Record& operator=(T const& val)	{return set(val);	}

		constexpr uint16 address() const {return memaddr;}

	private:
		uint16 const memaddr;
	};

	template <class T>
	struct Record<T, CacheType::BCT_CACHE_SAVE_ON_SET> {
		Record(uint16 const address):
			memaddr(address) {}

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

	template <class T>
	struct Record<T, CacheType::BCT_CACHE_MANUAL_SAVE> {
		Record(uint16 const address):
			memaddr(address) {}

		T get() const {
			return val;
		}

		void fetch(T& out) const {
			out = val;
		}

		void set(T const& v) {
			val = v;
		}

		void save() {EEPROM.put(memaddr, val);}

		inline operator T() const				{return get();		}
		inline Record& operator=(T const& val)	{return set(val);	}

		constexpr uint16 address() const {return memaddr;}

	private:
		uint16 const memaddr;
		T val;
	};

	template <class T>
	struct Record<T, CacheType::BCT_CACHE_SAVE_ON_DTOR> {
		Record(uint16 const address):
			memaddr(address) {}

		~Record() {save();}

		T get() const {
			return val;
		}

		void fetch(T& out) const {
			out = val;
		}

		void set(T const& v) {
			val = v;
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
