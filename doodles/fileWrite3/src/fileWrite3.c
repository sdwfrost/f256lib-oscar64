
#include "f256lib.h"

int main(int argc, char *argv[]) {
	uint8_t *fileNum =0;
	uint8_t binData[]={0xDE,0xAD,0xBE,0xEF};
	size_t binLen = sizeof(binData);

	fileNum = fileOpen("coucou.bin","w");
	fileWrite(binData, sizeof(uint8_t), binLen, fileNum);
	fileWrite(binData, sizeof(uint8_t), binLen, fileNum);
	fileWrite(binData, sizeof(uint8_t), binLen, fileNum);
	fileWrite(binData, sizeof(uint8_t), binLen, fileNum);
	fileWrite(binData, sizeof(uint8_t), binLen, fileNum);
	fileClose(fileNum);

	printf("hit space to quit");
	kernelWaitKey();

	return 0;
}
