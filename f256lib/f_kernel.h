/*
 *	Copyright (c) 2024 Scott Duensing, scott@kangaroopunch.com
 *	Adapted for oscar64.
 */


#ifndef KERNEL_H
#define KERNEL_H
#ifndef WITHOUT_KERNEL


#include "f256lib.h"


// Compute the offset of a member within the events struct (for event type matching)
#define kernelEvent(member)   (unsigned int)(&((struct events *)0)->member)

// Compute the kernel call vector address from the call struct at $FF00
#define kernelVector(member)  (unsigned int)(&((struct call *)0xff00)->member)

// Self-modifying code trampoline for kernel calls.
// _kern_target holds the JSR target address; _kernelCallRaw patches and calls it.
extern unsigned int _kern_target;


// Extern declarations for global kernel state
extern char                _kernelError;
extern struct event_t      kernelEventData;
extern struct call_args   *kernelArgs;

// Alias for compatibility with original code
#define kernelError _kernelError


// The __asm trampoline: patches JSR target, calls it,
// captures return value in A and carry flag in kernelError.
// Named __asm functions are called via jsr from inline assembly.
__asm _kernelCallRaw
{
	lda _kern_target
	sta kcjsr + 1
	lda _kern_target + 1
	sta kcjsr + 2
kcjsr:
	jsr $0000
	tax
	lda #0
	ror
	sta _kernelError
	txa
	rts
}

// C wrapper that sets target and invokes the asm trampoline.
// Returns the value left in A by the kernel call.
static char _kernelCallWrapper(void) {
	return __asm {
		jsr _kernelCallRaw
		sta accu
	};
}

#define kernelCall(fn)  (_kern_target = kernelVector(fn), _kernelCallWrapper())

// kernelNextEvent: clear the event type and call NextEvent
static char kernelNextEvent(void) {
	kernelEventData.type = 0;
	return kernelCall(NextEvent);
}


typedef struct event_t   kernelEventT;
typedef struct call_args kernelArgsT;


// Timer unit constants (from mu0nlibs/muUtils)
#define TIMER_FRAMES   0
#define TIMER_SECONDS  1


unsigned char kernelGetPending(void);
void kernelReset(void);

// Timer and wait utilities (from mu0nlibs/muUtils)
bool    kernelSetTimer(const struct timer_t *timer);
uint8_t kernelGetTimerAbsolute(uint8_t units);
void    kernelPause(uint8_t frames);
void    kernelWaitKey(void);


#pragma compile("f_kernel.c")


#endif
#endif // KERNEL_H
