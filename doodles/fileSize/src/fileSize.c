#include "f256lib.h"

#define FILE_COOKIE 21


void backgroundSetup(void) {
	POKE(MMU_IO_CTRL, 0x00);
	POKE(VKY_MSTR_CTRL_0, 0b00101111);
	POKE(VKY_MSTR_CTRL_1, 0b00000100);
	POKE(VKY_LAYER_CTRL_0, 0b00000001);
	POKE(VKY_LAYER_CTRL_1, 0b00000010);
	POKE(0xD00D, 0x00);
	POKE(0xD00E, 0x00);
	POKE(0xD00F, 0x00);

	bitmapSetActive(0);
	bitmapSetCLUT(0);

	bitmapSetVisible(0, false);
	bitmapSetVisible(1, false);
	bitmapSetVisible(2, false);
}


int main(int argc, char *argv[]) {
	uint16_t fileID = 0;
	uint32_t sizeSoFar = 0;

	backgroundSetup();

	kernelArgs->u.file.open.drive = 0;
	kernelArgs->u.common.buf = "ronald.mp3";
	kernelArgs->u.common.buflen = 11;
	kernelArgs->u.file.open.mode = 0;
	kernelArgs->u.file.open.cookie = FILE_COOKIE;

	kernelCall(File.Open);

	while (true) {
		kernelNextEvent();
		if (kernelEventData.type == kernelEvent(file.OPENED)) {
			if (kernelEventData.u.file.cookie == FILE_COOKIE) {
				fileID = kernelEventData.u.file.stream;
				break;
			}
		}
	}

	printf("\nThe file was opened! Now reading stream ID=%d...", fileID);

	kernelArgs->u.file.seek.stream = fileID;
	kernelArgs->u.file.seek.offset = -1L;

	kernelCall(File.Read);
	while (true) {
		kernelNextEvent();
		if (kernelEventData.type == kernelEvent(file.DATA)) {
			sizeSoFar += kernelEventData.u.file.u.data.delivered;

			kernelArgs->u.file.read.stream = fileID;
			kernelArgs->u.file.read.buflen = 255;
			kernelCall(File.Read);
		}

		#undef EOF
		if (kernelEventData.type == kernelEvent(file.EOF)) {
		#define EOF (-1)
			sizeSoFar += kernelEventData.u.file.u.data.delivered;
			break;
		}
	}

	printf("\n%lu bytes read", sizeSoFar);
	printf("\nHit space to end.");
	kernelWaitKey();
	return 0;
}
