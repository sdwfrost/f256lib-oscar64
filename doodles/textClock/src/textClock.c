#include "f256lib.h"

byte bcdToDec(byte bcd) {
	uint16_t tens = (bcd >> 4) & 0xF;
	uint16_t units = bcd & 0xF;
	return LOW_BYTE(tens * 10 + units);
}

int main(int argc, char *argv[]) {
	struct rtc_time_t time_data;
	byte oldsec = 0;

	textClear();
	textSetDouble(true, true);
	while (true) {
		kernelArgs->u.common.buf = &time_data;
		kernelArgs->u.common.buflen = sizeof(struct rtc_time_t);
		kernelCall(Clock.GetTime);

		if (time_data.seconds != oldsec) {
			oldsec = time_data.seconds;
			textGotoXY(10, 10);
			textPrint("                         ");
			textGotoXY(10, 10);
			printf("month %d day %d %d:%d:%d",
				bcdToDec(time_data.month),
				bcdToDec(time_data.day),
				bcdToDec(time_data.hours),
				bcdToDec(time_data.minutes),
				bcdToDec(time_data.seconds));
		}
	}
	return 0;
}
