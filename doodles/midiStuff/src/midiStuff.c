/*
 *	MIDI test/explorer utility.
 *	Ported from F256KsimpleCdoodles for oscar64.
 *	Uses muUtils for setTimer/getTimerAbsolute.
 *	Local note on/off functions renamed to avoid conflict with f256lib's midiNoteOn/midiNoteOff.
 */

#define MIDI_CTRL 	   0xDDA0
#define MIDI_FIFO 	   0xDDA1

#define MIDI_FIFO_ALT 	    0xDDB1

#define MIDI_RXD 	   0xDDA2
#define MIDI_RXD_COUNT 0xDDA3
#define MIDI_TXD       0xDDA4
#define MIDI_TXD_COUNT 0xDDA5

#include "f256lib.h"

 //TIMER_TEXT for the 1-frame long text refresh timer for data display
 //TIMER_NOTE is for a midi note timer
#define TIMER_TEXT_COOKIE 0
#define TIMER_NOTE_COOKIE 1
#define TIMER_TEXT_DELAY 1
#define TIMER_NOTE_DELAY 2

struct timer_t midiTimer, refTimer; //timer_t structure for setting timer through the kernel

uint16_t note = 0x36, oldnote; /*note is the current midi hex note code to send. oldnote keeps the previous one so it can be Note_off'ed away after the timer expires, or a new note is called*/
uint16_t prgInst = 0; /* program change value, the MIDI instrument number */
bool wantVS = true;

struct midi_uart {
	uint8_t status;
	uint8_t data;
	uint16_t bytes_in_rx;
	uint16_t bytes_in_tx;
} myMIDIsnapshot, *myMIDIptr;

void setup()
{
	textClear();
	textDefineForegroundColor(0,0xff,0xff,0xff);
    textGotoXY(0,0); textPrint("running a MIDI test\nLEFT/RIGHT changes note pitch; UP/DOWN changes instrument; Space to play a note");
	textGotoXY(0,2); textPrint("Plug in a midi controller in the MIDI in port and play!");
	textGotoXY(0,4); textPrint("0xDDA0");
	textGotoXY(0,5); textPrint("0xDDA1");
	textGotoXY(0,6); textPrint("0xDDA2");
	textGotoXY(0,7); textPrint("0xDDA3");
	textGotoXY(0,8); textPrint("0xDDA4");
	textGotoXY(0,9); textPrint("0xDDA5");

	textGotoXY(0,11); textPrint("Instrument: ");
	textGotoXY(0,12); textPrint("Note: ");

	midiTimer.units = TIMER_SECONDS;
	midiTimer.absolute = TIMER_NOTE_DELAY;
    midiTimer.cookie = TIMER_NOTE_COOKIE;

	refTimer.units = TIMER_FRAMES;
	refTimer.absolute = TIMER_TEXT_DELAY;
	refTimer.cookie = TIMER_TEXT_COOKIE;

	kernelSetTimer(&midiTimer);
	kernelSetTimer(&refTimer);

	POKE(MIDI_FIFO, 0xC0);
	POKE(MIDI_FIFO, 0);
}

void refreshPrints()
{

	textGotoXY(10,4); printf("%02x  ",PEEK(0xDDA0));
	textGotoXY(10,6); printf("%02x  ",PEEK(0xDDA2));
	textGotoXY(10,9); printf("%02x  ",PEEK(0xDDA5) & 0x0F);

	textGotoXY(13,11); printf(" %d",prgInst);
	textGotoXY(13,12); printf(" %d",note);
}

void localMidiNoteOff()
{
	//Send a Note_Off midi command for the previously ongoing note
    POKE(MIDI_FIFO, 0x80);
	POKE(MIDI_FIFO, oldnote);
    POKE(MIDI_FIFO, 0x4F);
}
void localMidiNoteOn()
{
	//Send a Note_On midi command on channel 0
	POKE(MIDI_FIFO, 0x90);
	POKE(MIDI_FIFO, note);
	POKE(MIDI_FIFO, 0x4F);
	//keep track of that note so we can Note_Off it when needed
	oldnote = note;
}
void commandNote()
{
	uint8_t result;
	localMidiNoteOff();
	//Prepare the next note timer
	result = kernelGetTimerAbsolute(TIMER_SECONDS);
	//the following line can be used to check absolute timer delays
	//printf(" result: %d %d       ",result , result + TIMER_NOTE_DELAY);
	midiTimer.absolute = result + TIMER_NOTE_DELAY;
	kernelSetTimer(&midiTimer);
	localMidiNoteOn();
}
void prgChange(bool wantUpOrDown)
{
	if(wantUpOrDown) prgInst++;
	else prgInst--;
	POKE(MIDI_FIFO, 0xC0);
	POKE(MIDI_FIFO, prgInst);
}

void injectChar(uint8_t x, uint8_t y, uint8_t theChar)
{
		POKE(0x0001,0x02); //set io_ctrl to 2 to access text screen
		POKE(0xC000 + 40 * y + x, theChar);
		POKE(0x0001,0x00);  //set it back to default
}


int main(int argc, char *argv[]) {
	uint16_t toDo;
	uint8_t x=0,y=2,i;
	uint8_t recByte, activity=0;
	//setup();
	textClear();
	POKE(1,0);

	printf("e0 received activity: "); //x=22 y=0
	setup();
	textGotoXY(x,y);
	while(true)
        {
		if(!(PEEK(MIDI_CTRL) & 0x02)) //rx not empty
			{
				toDo = PEEKW(MIDI_RXD) & 0x0FFF; //discard top 4 bits of MIDI_RXD+1
				textGotoXY(10,7); printf("%02x  ",PEEK(0xDDA3) & 0x0F);
				textGotoXY(10,8); printf("%02x  ",PEEK(0xDDA4));
				textGotoXY(10,5);
				printf("                                 ");
				textGotoXY(10,5);
				for(i=0; i<toDo; i++)
				{
					recByte=PEEK(MIDI_FIFO);
					printf("%02x  ",recByte);
					POKE(MIDI_FIFO, recByte);
				}
			}

		kernelNextEvent();
        if(kernelEventData.type == kernelEvent(timer.EXPIRED))
            {
			switch(kernelEventData.u.timer.cookie)
				{
				case TIMER_TEXT_COOKIE:
					refreshPrints();
					refTimer.absolute = kernelGetTimerAbsolute(TIMER_FRAMES) + TIMER_TEXT_DELAY;
					kernelSetTimer(&refTimer);
					break;
				case TIMER_NOTE_COOKIE:
					localMidiNoteOff();
					break;
				}
            }
		else if(kernelEventData.type == kernelEvent(key.PRESSED))
			{
				switch(kernelEventData.u.key.raw)
				{
					case 0xb6: //up
						if(prgInst < 127) prgChange(true);
						break;
					case 0xb7: //down
						if(prgInst > 0) prgChange(false);
						break;
					case 0xb8: //left
						if(note > 0) note--;
						break;
					case 0xb9: //right
						if(note < 127) note++;
						break;
					case 32: //space
						commandNote();
						break;
				}
				//the following line can be used to get keyboard codes
				//printf(" %d",kernelEventData.u.key.raw);
			}

        }
return 0;
}
