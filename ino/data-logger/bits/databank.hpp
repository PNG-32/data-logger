#ifndef BITS_DATABANK_H
#define BITS_DATABANK_H

#include <EEPROM.h>

#include "types.hpp"

/// @brief EEPROM size.
constexpr uint16 const EEPROM_SIZE = 1024;

/// @brief Helper classes & functions.
namespace Bits {
	/// @brief EEPROM data bank.
	/// @tparam T Entry type.
	/// @tparam MAX Possible entry limit.
	template <class T, uint16 MAX = 64>
	struct DataBank {
		/// @brief Entry type.
		using EntryType = T;

		/// @brief Data bank header.
		struct [[gnu::align(1)]] Header {
			uint32	exists		: 1;
			uint32	entryStart	: 12;
			uint32	entryCount	: 21;

			/// @brief Maximum accessible amount of entries.
			constexpr static uint32 const MAX_ENTRIES = (2ull << 19ull);

			static_assert(MAX_ENTRIES > 0);
			
			/// @brief Constructs a header from a given EEPROM location.
			constexpr static Header fromLocation(eeprom_address const location) {
				return {
					true,
					location + sizeof(Header),
					0
				};
			}
		};

		/// @brief Entry size.
		constexpr static usize const	ENTRY_SIZE		= sizeof(EntryType);
		/// @brief Header size.
		constexpr static usize const	HEADER_SIZE		= sizeof(Header);

		/// @brief Logical maximum amount of entries.
		constexpr static uint32 const	MAX_SIZE		= MAX;
		/// @brief Accessbible maximum amount of entries.
		constexpr static uint32 const	MAX_CAPACITY	= ((MAX_SIZE < Header::MAX_ENTRIES) ? MAX_SIZE : Header::MAX_ENTRIES);
		/// @brief True maximum amount of entries.
		constexpr static uint32 const	MAX_ENTRIES		= (EEPROM_SIZE - HEADER_SIZE) / ENTRY_SIZE;

		static_assert(MAX_ENTRIES	> 0);
		static_assert(MAX_CAPACITY	> 0);
		static_assert(ENTRY_SIZE	> 0);

		/// @brief Constructs the data bank from a memory location.
		/// @param location Memory location the data bank is stored in. By default, it is the beginning of the EEPROM (`0`).
		DataBank(eeprom_address const location = 0):
			headerLocation(location),
			header(Header::fromLocation(location)) {}
		
		/// @brief initializes the data bank.
		void begin() {
			if (EEPROM.read(headerLocation))
				EEPROM.get(headerLocation, header);
		}

		/// @brief Constructs the data bank from a header and a memory location.
		/// @param header Data bank header to use.	
		/// @param location Memory location the data bank is stored in.
		DataBank(Header const& header, eeprom_address const location):
			headerLocation(location),
			header(header) {
			updateHeader();
		}

		/// @brief Indexing operator overloading.
		EntryType operator[](uint8 const index) const {
			return get(index);
		}

		/// @brief Returns an entry at a given index.
		/// @param index Entry index.
		/// @return Entry at given location.
		EntryType get(uint8 const index) const {
			EntryType e;
			fetch(index, e);
			return e;
		}

		/// @brief Fetches an entry at a given index.
		/// @param index Entry index.
		/// @param output Where to store the fetched entry.
		void fetch(uint8 const index, EntryType& output) const {
			if (index < size())
				EEPROM.get((header.entryStart + index * ENTRY_SIZE), output);
		}

		/// @brief Modifies an entry at a given index.
		/// @param index Entry index.
		/// @param entry Value to modify entry with.			
		void set(uint8 const index, EntryType const& entry) {
			if (index < size())
				EEPROM.put((header.entryStart + index * ENTRY_SIZE), entry);
		}

		/// @brief Adds an entry to the end of the bank.
		/// @param entry Entry to add.
		void push(EntryType const& entry) {
			if (size() < MAX_ENTRIES && size() < MAX_CAPACITY) {
				++header.entryCount;
				set(size() - 1, entry);
				updateHeader();
			}
		}

		/// @brief Pops an entry from the end of the bank.
		void fastPop() {
			if (!empty()){
				--header.entryCount;
				updateHeader();
			}
		}

		/// @brief Pops an entry from the end of the bank.
		/// @return Entry at end of bank.
		EntryType pop() {
			EntryType e;
			if (!empty()) {
				fetch(size() - 1, e);
				--header.entryCount;
				updateHeader();
			}
			return e;
		}

		/// @brief Shifts all the entries of the bank one entry back.
		void fastShiftDown() {
			if (empty()) return;
			auto sz = header.entryCount-1;
			EntryType e;
			while (sz > 0) {
				fetch(sz, e);
				set(sz-1, e);
				--sz;
			}
			--header.entryCount;
			updateHeader();
		}

		/// @brief Shifts all the entries of the bank one entry back.
		/// @return Entry previously located at the beginning of the bank.		
		EntryType shiftDown() {
			EntryType e;
			if (!empty())
				fetch(0, e);
			fastShiftDown();
			return e;
		}
		
		/// @brief Empties the bank.
		void clear() {
			header.entryCount = 0;
			updateHeader();
		}

		/// @brief Records an entry in the entry bank. If the bank is full, removes the first entry in the bank.
		/// @param entry Entry to record.
		void record(EntryType const& entry) {
			if (size() >= MAX_ENTRIES)
				fastShiftDown();
			push(entry);
		}

		/// @brief Returns the EEPROM address of the bank.
		/// @return Location of bank.		
		inline eeprom_address	address() const	{return headerLocation;		}
		/// @brief Returns the amount of entries in the bank.
		/// @return Entry count.
		inline uint32			size() const	{return header.entryCount;	}
		/// @brief Returns whether the bank is empty.
		/// @return Whether bank is empty.
		inline bool				empty() const	{return size() == 0;		}

	private:
		/// @brief Header location.
		eeprom_address headerLocation = 0;
		/// @brief Bank header.	
		Header header = Header::fromLocation(headerLocation);

		/// @brief Updates the header in the EEPROM.
		void updateHeader() {
			EEPROM.put(headerLocation, header);
		}
	};
}

#endif