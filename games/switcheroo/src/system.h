#ifndef PLATFORM_SYSTEM_H
#define PLATFORM_SYSTEM_H

void platform_bootstrap(void);
void platform_shutdown(void);
void platform_idle(void);

#pragma compile("system.c")
#endif /* PLATFORM_SYSTEM_H */
