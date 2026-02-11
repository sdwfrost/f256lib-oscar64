/*
 *   This file is part of the TinyCore 6502 MicroKernel, Copyright 2022 Jessie
 *   Oberreuter <joberreu@moselle.com>. As with the Linux Kernel Exception to
 *   the GPL3, programs built to run on the MicroKernel are expected to
 *   include this file. Doing so does not effect their license status.
 *
 *   Adapted for oscar64 by removing #pragma push_macro/pop_macro,
 *   renaming struct time_t to struct rtc_time_t,
 *   naming anonymous unions for C mode compatibility,
 *   and renaming 'delete' to 'del' to avoid C++ keyword conflict.
 */

#ifndef KERNEL_API_H
#define KERNEL_API_H

#include <stdint.h>

// Save and undefine EOF to avoid conflict with struct events.file.EOF
#ifdef EOF
#define _SAVED_EOF EOF
#undef EOF
#endif

struct call {  // Mount at $ff00

    long NextEvent;
    long ReadData;
    long ReadExt;
    long Yield;
    long Putch;
    long RunBlock;
    long RunNamed;
    long reserved;

    struct {
        long List;
        long GetName;
        long GetSize;
        long Read;
        long Write;
        long Format;
        long Export;
    } BlockDevice;

    struct {
        long List;
        long GetSize;
        long MkFS;
        long CheckFS;
        long Mount;
        long Unmount;
        long ReadBlock;
        long WriteBlock;
    } FileSystem;

    struct {
        long Open;
        long Read;
        long Write;
        long Close;
        long Rename;
        long Delete;
        long Seek;
    } File;

    struct {
        long Open;
        long Read;
        long Close;
        long MkDir;
        long RmDir;
    } Directory;

    long gate;

    struct {
        long GetIP;
        long SetIP;
        long GetDNS;
        long SetDNS;
        long SendICMP;
        long Match;

        struct {
            long Init;
            long Send;
            long Recv;
        } UDP;

        struct {
            long Open;
            long Accept;
            long Reject;
            long Send;
            long Recv;
            long Close;
        } TCP;
    } Network;

    struct {
        long Reset;
        long GetSize;
        long DrawRow;
        long DrawColumn;
    } Display;

    struct {
        long GetTime;
        long SetTime;
        long vectors816_1;
        long vectors816_2;
        long vectors816_3;
        long SetTimer;
    } Clock;
};

// Kernel Call Arguments; mount at $f0

struct events_t {
    struct event_t *event;
    char            pending;
};

struct common_t {
    char        dummy[8-sizeof(struct events_t)];
    const void *ext;
    uint8_t     extlen;
    const void *buf;
    uint8_t     buflen;
    void *      internal;
};

struct fs_mkfs_t {
    uint8_t  drive;
    uint8_t  cookie;
};

// Changed from struct with anonymous union to plain union
union fs_u {
    struct fs_mkfs_t  format;
    struct fs_mkfs_t  mkfs;
};

struct fs_open_t {
    uint8_t drive;
    uint8_t cookie;
    uint8_t mode;
};

enum fs_open_mode {
    READ,
    WRITE,
    APPEND
};

struct fs_read_t {
    uint8_t stream;
    uint8_t buflen;
};

struct fs_write_t {
    uint8_t stream;
};

struct fs_seek_t {
	uint8_t  stream;
	uint32_t offset;
};

struct fs_close_t {
	uint8_t stream;
};

struct fs_rename_t {
    uint8_t drive;
    uint8_t cookie;
};

struct fs_delete_t {
    uint8_t drive;
    uint8_t cookie;
};

// Changed from struct with anonymous union to plain union
// Renamed 'delete' to 'del' to avoid C++ keyword conflict
union file_u {
    struct fs_open_t    open;
    struct fs_read_t    read;
    struct fs_write_t   write;
    struct fs_seek_t    seek;
    struct fs_close_t   close;
    struct fs_rename_t  rename;
    struct fs_delete_t  del;
};

struct dir_open_t {
    uint8_t drive;
    uint8_t cookie;
};

struct dir_read_t {
    uint8_t stream;
    uint8_t buflen;
};

struct dir_close_t {
    uint8_t stream;
};

// Changed from struct with anonymous union to plain union
union dir_u {
    struct dir_open_t   open;
    struct dir_read_t   read;
    struct dir_close_t  close;
    struct dir_open_t   mkdir;
    struct dir_open_t   rmdir;
};

struct display_t {
    uint8_t x;
    uint8_t y;
};

struct net_init_t {
	uint16_t source_port;
	uint16_t dest_port;
	long     dest_ip;
};

struct net_send_recv_t {
	uint8_t accepted;
};

// Changed from struct with anonymous union to plain union
union net_u {
	struct net_init_t       net_init;
	struct net_send_recv_t  net_send_recv;
};

struct timer_t {
	uint8_t units;
	uint8_t absolute;
	uint8_t cookie;
};

// Renamed from time_t to avoid conflict with <time.h>
struct rtc_time_t {
	uint8_t century;
	uint8_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;
	uint8_t centis;
};


struct call_args {
    struct events_t events;
    union {
        struct common_t   common;
        union  fs_u       fs;
        union  file_u     file;
        union  dir_u      directory;
        struct display_t  display;
        union  net_u      net;
        struct timer_t    timer;
    } u;
};


// Events

struct events {
    uint16_t reserved;
    uint16_t deprecated;
    uint16_t GAME;
    uint16_t DEVICE;

    struct {
        uint16_t PRESSED;
        uint16_t RELEASED;
    } key;

    struct {
        uint16_t DELTA;
        uint16_t CLICKS;
    } mouse;

    struct {
        uint16_t NAME;
        uint16_t SIZE;
        uint16_t DATA;
        uint16_t WROTE;
        uint16_t FORMATTED;
        uint16_t ERROR;
    } block;

    struct {
        uint16_t SIZE;
        uint16_t CREATED;
        uint16_t CHECKED;
        uint16_t DATA;
        uint16_t WROTE;
        uint16_t ERROR;
    } fs;

    struct {
        uint16_t NOT_FOUND;
        uint16_t OPENED;
        uint16_t DATA;
        uint16_t WROTE;
        uint16_t EOF;
        uint16_t CLOSED;
        uint16_t RENAMED;
        uint16_t DELETED;
        uint16_t ERROR;
        uint16_t SEEK;
    } file;

    struct {
        uint16_t OPENED;
        uint16_t VOLUME;
        uint16_t FILE;
        uint16_t FREE;
        uint16_t EOF;
        uint16_t CLOSED;
        uint16_t ERROR;
        uint16_t CREATED;
        uint16_t DELETED;
    } directory;

    struct {
        uint16_t TCP;
        uint16_t UDP;
    } net;

	struct {
		uint16_t EXPIRED;
	} timer;

	struct {
		uint16_t TICK;
	} clock;
};


struct event_key_t {
    uint8_t keyboard;
    uint8_t raw;
    char    ascii;
    char    flags;
};

struct event_mouse_delta_t {
    char     x;
    char     y;
    char     z;
    uint8_t  buttons;
};

struct event_mouse_clicks_t {
    uint8_t inner;
    uint8_t middle;
    uint8_t outer;
};

// Changed from struct with anonymous union to plain union
union event_mouse_u {
    struct event_mouse_delta_t  delta;
    struct event_mouse_clicks_t clicks;
};

struct event_game_t {
	uint8_t game0;
	uint8_t game1;
};

struct event_fs_data_t {
    uint8_t requested;
    uint8_t delivered;
};

struct event_fs_wrote_t {
    uint8_t requested;
    uint8_t delivered;
};

struct event_file_t {
    uint8_t stream;
    uint8_t cookie;
    union {
        struct event_fs_data_t  data;
        struct event_fs_wrote_t wrote;
    } u;
};

struct event_dir_vol_t {
    uint8_t len;
    uint8_t flags;
};

struct event_dir_file_t {
    uint8_t len;
    uint8_t flags;
};

struct event_dir_free_t {
    uint8_t flags;
};

struct dir_ext_t {
    uint32_t free;
};

struct event_dir_t {
    uint8_t stream;
    uint8_t cookie;
    union {
        struct event_dir_vol_t  volume;
        struct event_dir_file_t file;
        struct event_dir_free_t free;
    } u;
};

struct event_udp_t {
    uint8_t token;
};

struct event_tcp_t {
    uint8_t len;
};

struct event_timer_t {
	uint8_t value;
	uint8_t cookie;
};

struct event_t {
    uint8_t type;
    uint8_t buf;
    uint8_t ext;
    union {
        struct event_key_t    key;
        union  event_mouse_u  mouse;
        struct event_game_t   game;
        struct event_udp_t    udp;
        struct event_tcp_t    tcp;
        struct event_file_t   file;
        struct event_dir_t    directory;
        struct event_timer_t  timer;
    } u;
};

// Restore EOF
#ifdef _SAVED_EOF
#define EOF _SAVED_EOF
#undef _SAVED_EOF
#endif

#endif // KERNEL_API_H
