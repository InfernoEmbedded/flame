/*
 * Declare a pure virtual function implementation here once,
 * it won't get linked unless there's a reference to it needed.
 */
#include <avr/interrupt.h>
extern "C" __attribute__ ((noreturn)) void __cxa_pure_virtual() {
	cli();
	for (;;);
}
