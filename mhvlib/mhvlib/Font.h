#ifndef MHV_FONT_H
#define MHV_FONT_H

#include <inttypes.h>
#include <avr/pgmspace.h>

/* Font layout for use with display classes
 * A font contains a table font data, stored in program memory
 * Seperate tables contain information regarding widths & offsets into the table for each character
 *
 * The font data is stored MSB first (big endian) for multibyte fonts. The most significant bit is
 * at the bottom of the character, while the least significant is at the top
 */

namespace mhvlib {

struct font {
	uint8_t		maxWidth;		// maximum width of a character
	uint8_t		maxHeight;		// maximum height of a character
	char		firstChar;		// the first character in the font
	uint8_t		charCount;		// the number of characters in the font
	char		unknown;		// which character to render unknowns to
	uint8_t		columnBytes;	// the number of bytes used for a column
	const uint8_t *widths;
	const uint16_t *offsets;
	const uint8_t *fontData; // most significant bit at the bottom
};
typedef struct font FONT;

}
#endif /* MHV_FONT_H */
