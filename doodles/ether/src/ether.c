/*
 *	WiFi / Ethernet test.
 *	Ported from F256KsimpleCdoodles for oscar64.
 */

#include "f256lib.h"

#define WIFI_CTRL      0xDD80
#define WIFI_DATA      0xDD81
#define WIFI_RX_F      0xDD82
#define WIFI_TX_F      0xDD84

#define STDPAUSE       5


void uart_putc(char c) {
	POKE(WIFI_DATA, c);
}

char uart_getc(void) {
	if (PEEKW(WIFI_RX_F)) return PEEK(WIFI_DATA);
	return 0;
}

void uart_puts(const char *s) {
	while (*s) uart_putc(*s++);
}

static bool wiz_wait_ok(void) {
	kernelPause(30);
	return true;
}

static bool wiz_cmd(const char *cmd) {
	uart_puts(cmd);
	uart_puts("\r\n");
	return wiz_wait_ok();
}

bool wiz_init(void) {
	if (!wiz_cmd("AT")) return false;
	kernelPause(STDPAUSE);
	if (!wiz_cmd("AT+RST")) return false;
	kernelPause(STDPAUSE);
	if (!wiz_cmd("AT+CWMODE=1")) return false;
	kernelPause(STDPAUSE);
	if (!wiz_cmd("AT+CWDHCP=1,1")) return false;
	kernelPause(STDPAUSE);
	return true;
}

bool wiz_join(const char *ssid, const char *pass) {
	uart_puts("AT+CWJAP=\"");
	uart_puts(ssid);
	uart_puts("\",\"");
	uart_puts(pass);
	uart_puts("\"\r\n");
	return wiz_wait_ok();
}

bool wiz_tcp_open(void) {
	uart_puts("AT+CIPMUX=1\r\n");
	wiz_wait_ok();
	uart_puts("AT+CIPSTART=0,\"TCP\",\"192.168.1.177\",80\r\n");
	return wiz_wait_ok();
}

bool wiz_tcp_send(const char *msg) {
	char lenbuf[8];
	int len = strlen(msg);

	uart_puts("AT+CIPSEND=0,");
	sprintf(lenbuf, "%d", len);
	uart_puts(lenbuf);
	uart_puts("\r\n");

	while (uart_getc() != '>');

	uart_puts(msg);
	return wiz_wait_ok();
}

uint8_t wifiTest(void) {
	printf("Starting WizFi360...\r\n");

	if (!wiz_init()) {
		printf("Init failed\r\n");
		return 1;
	}

	if (!wiz_join("BatCave", "leocestleplusbeau")) {
		printf("WiFi join failed\r\n");
		return 1;
	}

	if (!wiz_tcp_open()) {
		printf("TCP open failed\r\n");
		return 1;
	}

	wiz_tcp_send("hello");
	return 0;
}

int main(int argc, char *argv[]) {

	POKE(MMU_IO_CTRL, 0x00);
	POKE(VKY_MSTR_CTRL_0, 0b00000111);
	POKE(VKY_MSTR_CTRL_1, 0b00010000);
	POKE(WIFI_CTRL, 0x00);
	kernelPause(10);

	uint16_t a = 1;
	uint16_t b = 188;
	uint16_t c = 0;

	c = mathUnsignedMultiply(a, b);
	uint16_t d = 0;
	mathUnsignedDivisionRemainder(c, 100, &d);

	printf("float c is %d remainder %d", mathUnsignedDivision(c, 100), d);
	getchar();
	while (true) {
		printf("\n-----START TEST\n");
		wifiTest();
		printf("\n-------END TEST\n");
		getchar();
	}
	return 0;
}
