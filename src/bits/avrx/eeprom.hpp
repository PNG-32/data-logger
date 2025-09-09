#ifndef BITS_AVRX_EEPROM_H
#define BITS_AVRX_EEPROM_H

#include "core.hpp"

/// @brief EEPROM facilities.
namespace Bits::AVRX::EEPROM {
	/// @brief EEPROM address type.
	using Address = word;
	/// @brief Reads a byte from EEPROM.
	/// @param address EEPROM address to read from.
	/// @brief Byte at the given address.
	[[gnu::pure, gnu::always_inline]]
	inline byte readByte(Address const address) {
		return eeprom_read_byte((ref<byte const>)address);
	}

	/// @brief Reads a word from EEPROM.
	/// @param address EEPROM address to read from.
	/// @brief Word at the given address.
	[[gnu::pure, gnu::always_inline]]
	inline word readWord(Address const address) {
		return eeprom_read_word((ref<word const>)address);
	}

	/// @brief Reads a value from EEPROM.
	/// @tparam T Value type.
	/// @param address EEPROM address to read from.
	/// @param out Output.
	template <class T>
	[[gnu::always_inline]]
	inline void readBlock(Address const address, T& out) {
		return eeprom_read_block(&out, (ref<void const>)address, sizeof(T));
	}

	/// @brief Reads a series of values from EEPROM.
	/// @tparam T Value type.
	/// @param address EEPROM address to start reading from.
	/// @param begin Beginning of value range.
	/// @param begin End of value range.
	template <class T>
	[[gnu::always_inline]]
	inline void readBlock(Address const address, ref<T> const begin, ref<T> const end) {
		eeprom_read_block(begin, (ref<void const>)address, (end - begin) * sizeof(T));
	}

	/// @brief Writes a byte to EEPROM.
	/// @param address EEPROM address to write to.
	/// @param value Byte to write.
	[[gnu::always_inline]]
	inline void writeByte(Address const address, byte const value) {
		eeprom_write_byte((uint8_t*)address, value);
	}

	/// @brief Writes a byte to EEPROM.
	/// @param address EEPROM address to write to.
	/// @param value Word to write.
	[[gnu::always_inline]]
	inline void writeWord(Address const address, word const value) {
		eeprom_write_word((uint16_t*)address, value);
	}
	
	/// @brief Writes a series of values to EEPROM.
	/// @tparam T Value type.
	/// @param address EEPROM address to start writing to.
	/// @param begin Beginning of value range.
	/// @param begin End of value range.
	template <class T>
	[[gnu::always_inline]]
	inline void writeBlock(Address const address, T const& value) {
		eeprom_write_block(&value, (pointer)address, sizeof(T));
	}

	/// @brief Writes a series of values to EEPROM.
	/// @tparam T Value type.
	/// @param address EEPROM address to start writing to.
	/// @param begin Beginning of value range.
	/// @param begin End of value range.
	template <class T>
	[[gnu::always_inline]]
	inline void writeBlock(Address const address, ref<T const> const begin, ref<T const> const end) {
		eeprom_write_block(begin, (pointer)address, (end - begin) * sizeof(T));
	}
}

#endif