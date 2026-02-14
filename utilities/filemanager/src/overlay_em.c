/*
 * overlay_em.c - Extended memory display (hex/text viewer)
 * Ported from F256-FileManager CC65 version
 *
 * Provides hex dump and text viewing of memory banks or loaded files.
 * Uses bank switching via SWAP_SLOT/SWAP_ADDR to read memory.
 */

#include "overlay_em.h"
#include "screen.h"
#include "strings.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/*****************************************************************************/
/*                               Definitions                                 */
/*****************************************************************************/

#define CH_LINE_BREAK			10
#define CH_LINE_RETURN			13

#define HEX_DISPLAY_NUM_CHARS_PER_ROW	16
#define HEX_DISPLAY_NUM_ROWS			59
#define PAGES_PER_BANK					32		// 8192/256


/*****************************************************************************/
/*                          File-scoped Variables                            */
/*****************************************************************************/

static byte em_temp_buffer[256];
static char em_line_buffer[82];		// 80 chars + possible overflow + null


/*****************************************************************************/
/*                       Private Function Prototypes                         */
/*****************************************************************************/

static void EM_ReadPageFromBank(byte bank_num, byte page_num, byte* dest);
static void EM_DrawHexByte(byte b, char* out);


/*****************************************************************************/
/*                       Private Function Definitions                        */
/*****************************************************************************/


// Read a 256-byte page from a physical bank into a destination buffer
static void EM_ReadPageFromBank(byte bank_num, byte page_num, byte* dest)
{
	byte	saved_bank;
	byte	actual_bank;
	uint16_t page_offset;
	uint16_t i;

	// Calculate which bank and offset within bank
	actual_bank = bank_num + (page_num / PAGES_PER_BANK);
	page_offset = (page_num % PAGES_PER_BANK) * 256;

	saved_bank = PEEK(SWAP_SLOT);
	POKE_MEMMAP(SWAP_SLOT, actual_bank);

	for (i = 0; i < 256; i++)
		dest[i] = PEEK(SWAP_ADDR + page_offset + i);

	POKE_MEMMAP(SWAP_SLOT, saved_bank);
}


// Convert a byte to two hex characters
static void EM_DrawHexByte(byte b, char* out)
{
	static const char hex_chars[] = "0123456789ABCDEF";
	out[0] = hex_chars[(b >> 4) & 0x0F];
	out[1] = hex_chars[b & 0x0F];
}


/*****************************************************************************/
/*                        Public Function Definitions                        */
/*****************************************************************************/


void EM_DisplayAsText(byte em_bank_num, byte num_pages, char* the_name)
{
	byte	i;
	byte	y;
	byte	user_input;
	bool	keep_going = true;
	byte	col;
	byte	buf_pos;

	y = 0;

	for (i = 0; i < num_pages && keep_going; i++)
	{
		uint16_t j;

		EM_ReadPageFromBank(em_bank_num, i, em_temp_buffer);

		for (j = 0; j < 256 && keep_going; j++)
		{
			byte ch = em_temp_buffer[j];

			if (y == 0)
			{
				textFillBox(0, 0, 79, 59, ' ', FILE_CONTENTS_FOREGROUND_COLOR, FILE_CONTENTS_BACKGROUND_COLOR);
				sprintf(global_string_buff1, "%s: %s", GetString(ID_STR_MSG_TEXT_VIEW_INSTRUCTIONS), the_name);
				textDrawStringAt(0, y, global_string_buff1, FILE_CONTENTS_ACCENT_COLOR, FILE_CONTENTS_BACKGROUND_COLOR);
				y = 2;
				col = 0;
				buf_pos = 0;
			}

			if (ch == CH_LINE_BREAK || ch == CH_LINE_RETURN)
			{
				// End current line and display
				em_line_buffer[buf_pos] = 0;
				textDrawStringAt(0, y, em_line_buffer, FILE_CONTENTS_FOREGROUND_COLOR, FILE_CONTENTS_BACKGROUND_COLOR);
				buf_pos = 0;
				col = 0;
				++y;
			}
			else if (ch == 0)
			{
				// Skip null bytes in text mode
			}
			else
			{
				if (buf_pos < 80)
				{
					em_line_buffer[buf_pos++] = ch;
					++col;
				}

				// Line wrap at 80 columns
				if (col >= MEM_TEXT_VIEW_BYTES_PER_ROW)
				{
					em_line_buffer[buf_pos] = 0;
					textDrawStringAt(0, y, em_line_buffer, FILE_CONTENTS_FOREGROUND_COLOR, FILE_CONTENTS_BACKGROUND_COLOR);
					buf_pos = 0;
					col = 0;
					++y;
				}
			}

			// Screen full?
			if (y >= MAX_TEXT_VIEW_ROWS_PER_PAGE)
			{
				// Wait for user input
				user_input = Screen_GetValidUserInput();

				if (user_input == ACTION_CANCEL || user_input == 'q')
				{
					keep_going = false;
				}
				else
				{
					y = 0;
				}
			}
		}
	}

	// Flush any remaining partial line
	if (buf_pos > 0 && keep_going)
	{
		em_line_buffer[buf_pos] = 0;
		textDrawStringAt(0, y, em_line_buffer, FILE_CONTENTS_FOREGROUND_COLOR, FILE_CONTENTS_BACKGROUND_COLOR);
	}

	// Let user see the last page
	if (keep_going)
		Screen_GetValidUserInput();
}


void EM_DisplayAsHex(byte em_bank_num, byte num_pages, char* the_name)
{
	byte		i;
	byte		y;
	byte		user_input;
	bool		keep_going = true;
	uint32_t	loc_in_file;

	// Determine starting address: if viewing a specific bank, show physical address
	if (em_bank_num == 0x14)
		loc_in_file = 0x0000;  // Viewing a loaded file
	else
		loc_in_file = (uint32_t)em_bank_num * 8192;

	y = 0;

	for (i = 0; i < num_pages && keep_going; i++)
	{
		byte rows_this_chunk;

		EM_ReadPageFromBank(em_bank_num, i, em_temp_buffer);
		rows_this_chunk = 0;

		while (rows_this_chunk < 16 && keep_going)  // 16 rows per 256-byte page
		{
			byte	n;
			byte	buf_idx;
			byte*	row_data = &em_temp_buffer[rows_this_chunk * HEX_DISPLAY_NUM_CHARS_PER_ROW];

			if (y == 0)
			{
				textFillBox(0, 0, 79, 59, ' ', FILE_CONTENTS_FOREGROUND_COLOR, FILE_CONTENTS_BACKGROUND_COLOR);
				sprintf(global_string_buff1, "%s: %s", GetString(ID_STR_MSG_HEX_VIEW_INSTRUCTIONS), the_name);
				textDrawStringAt(0, y, global_string_buff1, FILE_CONTENTS_ACCENT_COLOR, FILE_CONTENTS_BACKGROUND_COLOR);
				y = 2;
			}

			// Build the hex line: "$AABBCC  XX XX XX ... XX  CCCCCCCCCCCCCCCC"
			buf_idx = 0;

			// Address
			em_line_buffer[buf_idx++] = '$';
			EM_DrawHexByte((byte)((loc_in_file >> 16) & 0xFF), &em_line_buffer[buf_idx]); buf_idx += 2;
			EM_DrawHexByte((byte)((loc_in_file >> 8) & 0xFF), &em_line_buffer[buf_idx]); buf_idx += 2;
			EM_DrawHexByte((byte)(loc_in_file & 0xFF), &em_line_buffer[buf_idx]); buf_idx += 2;
			em_line_buffer[buf_idx++] = ' ';
			em_line_buffer[buf_idx++] = ' ';

			// Hex bytes
			for (n = 0; n < HEX_DISPLAY_NUM_CHARS_PER_ROW; n++)
			{
				EM_DrawHexByte(row_data[n], &em_line_buffer[buf_idx]);
				buf_idx += 2;
				em_line_buffer[buf_idx++] = ' ';
			}

			// Separator
			em_line_buffer[buf_idx++] = ' ';

			// ASCII representation
			for (n = 0; n < HEX_DISPLAY_NUM_CHARS_PER_ROW; n++)
			{
				byte ch = row_data[n];
				if (ch >= 32 && ch < 127)
					em_line_buffer[buf_idx++] = ch;
				else
					em_line_buffer[buf_idx++] = '.';
			}

			em_line_buffer[buf_idx] = 0;

			textDrawStringAt(0, y, em_line_buffer, FILE_CONTENTS_FOREGROUND_COLOR, FILE_CONTENTS_BACKGROUND_COLOR);

			loc_in_file += MEM_DUMP_BYTES_PER_ROW;
			++rows_this_chunk;
			++y;

			// Screen full?
			if (y >= MAX_TEXT_VIEW_ROWS_PER_PAGE)
			{
				user_input = Screen_GetValidUserInput();

				if (user_input == ACTION_CANCEL || user_input == 'q')
					keep_going = false;
				else
					y = 0;
			}
		}
	}

	if (keep_going)
		Screen_GetValidUserInput();
}


bool EM_SearchMemory(byte start_bank, byte start_page, byte start_byte, bool new_search)
{
	byte		search_bank;
	byte		search_page;
	byte		search_byte;
	uint32_t	find_location;
	byte		i;

	if (global_search_phrase_len == 0)
		return false;

	search_bank = start_bank;
	search_page = start_page;
	search_byte = start_byte;

	// If continuing from a previous search, advance by 1
	if (new_search == false)
	{
		if (search_byte < 255)
		{
			++search_byte;
		}
		else
		{
			search_byte = 0;
			if (search_page < PAGES_PER_BANK - 1)
				++search_page;
			else
			{
				search_page = 0;
				if (search_bank < NUM_MEMORY_BANKS - 1)
					++search_bank;
				else
					return false;
			}
		}
	}

	// Bank loop
	while (search_bank < NUM_MEMORY_BANKS)
	{
		// Page loop
		while (search_page < PAGES_PER_BANK)
		{
			EM_ReadPageFromBank(search_bank, search_page, em_temp_buffer);

			// Byte loop
			for (; search_byte < 256; search_byte++)
			{
				if (em_temp_buffer[search_byte] != (byte)global_search_phrase[0])
					continue;

				// First byte matched - check rest
				bool matched = true;
				for (i = 1; i < global_search_phrase_len; i++)
				{
					byte check_byte_idx = search_byte + i;
					byte check_page = search_page;
					byte check_bank = search_bank;

					// Handle crossing page/bank boundaries
					if (check_byte_idx >= 256)
					{
						check_byte_idx -= 256;
						check_page++;
						if (check_page >= PAGES_PER_BANK)
						{
							check_page = 0;
							check_bank++;
							if (check_bank >= NUM_MEMORY_BANKS)
							{
								matched = false;
								break;
							}
						}
					}

					// If we need to cross boundaries, read the next page
					if (search_byte + i >= 256)
					{
						byte cross_buf[256];
						EM_ReadPageFromBank(check_bank, check_page, cross_buf);
						if (cross_buf[check_byte_idx] != (byte)global_search_phrase[i])
						{
							matched = false;
							break;
						}
					}
					else
					{
						if (em_temp_buffer[search_byte + i] != (byte)global_search_phrase[i])
						{
							matched = false;
							break;
						}
					}
				}

				if (matched)
				{
					find_location = (uint32_t)search_bank * 8192 + (uint32_t)search_page * 256 + search_byte;
					sprintf(global_string_buff1, "%s at $%05lX (bank %u)",
						GetString(ID_STR_MSG_SEARCH_BANK_SUCCESS),
						find_location, search_bank);
					Buffer_NewMessage(global_string_buff1);
					return true;
				}
			}

			search_byte = 0;
			++search_page;
		}

		search_page = 0;
		++search_bank;
	}

	// Not found
	sprintf(global_string_buff1, "%s '%s'",
		GetString(ID_STR_MSG_SEARCH_BANK_FAILURE),
		global_search_phrase_human_readable);
	Buffer_NewMessage(global_string_buff1);
	return false;
}
