#include "f256lib.h"
#include <stdio.h>

// Print a byte as two hex digits
static void print_hex(unsigned char v)
{
	static const char hex[] = "0123456789ABCDEF";
	char buf[3];
	buf[0] = hex[v >> 4];
	buf[1] = hex[v & 0x0F];
	buf[2] = 0;
	textPrint(buf);
}

// Print the key event fields from kernelEventData
static void print_key_event(void)
{
	unsigned char raw   = kernelEventData.u.key.raw;
	char          ascii = kernelEventData.u.key.ascii;
	char          flags = kernelEventData.u.key.flags;
	unsigned char kb    = kernelEventData.u.key.keyboard;

	// Raw scan code
	textPrint("raw=0x");
	print_hex(raw);

	// ASCII value
	textPrint(" ascii=");
	if (ascii >= 32 && ascii < 127)
	{
		char buf[4];
		buf[0] = '\'';
		buf[1] = ascii;
		buf[2] = '\'';
		buf[3] = 0;
		textPrint(buf);
	}
	else
	{
		textPrint("0x");
		print_hex((unsigned char)ascii);
	}

	// Flags
	textPrint(" flags=0x");
	print_hex((unsigned char)flags);

	// Keyboard ID
	textPrint(" kb=");
	print_hex(kb);
}

// Test keyboardGetCharAsync mapping
static void print_mapped_char(char c)
{
	textPrint("  mapped=");
	if (c == KEY_UP)
		textPrint("KEY_UP");
	else if (c == KEY_DOWN)
		textPrint("KEY_DOWN");
	else if (c == KEY_LEFT)
		textPrint("KEY_LEFT");
	else if (c == KEY_RIGHT)
		textPrint("KEY_RIGHT");
	else if (c == 13)
		textPrint("ENTER");
	else if (c == 27)
		textPrint("ESC");
	else if (c >= 32 && c < 127)
	{
		char buf[4];
		buf[0] = '\'';
		buf[1] = c;
		buf[2] = '\'';
		buf[3] = 0;
		textPrint(buf);
	}
	else
	{
		textPrint("0x");
		print_hex((unsigned char)c);
	}
}

int main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	textSetDouble(true, false);
	textSetCursor(0);
	textClear();

	textDrawStringAt(0, 0, "=== KEYBOARD TEST ===", WHITE, BLACK);
	textDrawStringAt(0, 2, "Press keys to see raw kernel events", LIGHT_GRAY, BLACK);
	textDrawStringAt(0, 3, "and f256lib mapped values.", LIGHT_GRAY, BLACK);
	textDrawStringAt(0, 4, "Hold ESC for 2 seconds to quit.", LIGHT_GRAY, BLACK);

	char row = 6;
	unsigned char esc_count = 0;

	for (;;)
	{
		// Get raw kernel event
		kernelNextEvent();

		if (kernelEventData.type == kernelEvent(key.PRESSED))
		{
			// Scroll if near bottom
			if (row >= 29)
			{
				textScrollRowsUp(6, 29);
				row = 28;
			}

			textGotoXY(0, row);

			// Show raw event data
			print_key_event();

			// Now test the f256lib mapping by feeding the same
			// event through keyboardGetCharAsync's logic.
			// Since the event is already in kernelEventData, we
			// can check what keyboardGetCharAsync would return.
			char c = kernelEventData.u.key.ascii;
			if (c && !kernelEventData.u.key.flags)
			{
				// ASCII path
				print_mapped_char(c);
			}
			else
			{
				// Raw scancode path
				switch (kernelEventData.u.key.raw)
				{
					case 0xB6: print_mapped_char(KEY_UP); break;
					case 0xB7: print_mapped_char(KEY_DOWN); break;
					case 0xB8: print_mapped_char(KEY_LEFT); break;
					case 0xB9: print_mapped_char(KEY_RIGHT); break;
					case 0x94: print_mapped_char(13); break;
					case 0x95: print_mapped_char(27); break;
					default:
						textPrint("  mapped=(none)");
						break;
				}
			}

			textPrint("\n");
			row++;

			// Track ESC for exit
			if (kernelEventData.u.key.raw == 0x95)
			{
				esc_count++;
				if (esc_count >= 2)
					break;
			}
			else
				esc_count = 0;
		}
		else if (kernelEventData.type == kernelEvent(key.RELEASED))
		{
			// Ignore releases but reset ESC counter on non-ESC activity
		}
		else if (kernelEventData.type == 0)
		{
			// No event - yield to kernel
			kernelCall(Yield);
		}
	}

	textClear();
	textDrawStringAt(0, 0, "Test complete.", WHITE, BLACK);

	return 0;
}
