

#include <linux/serial_reg.h>
#include <mach/addr-map.h>
#include <asm/mach-types.h>

#define UART1_BASE	(APB_PHYS_BASE + 0x36000)
#define UART2_BASE	(APB_PHYS_BASE + 0x17000)
#define UART3_BASE	(APB_PHYS_BASE + 0x18000)

static volatile unsigned long *UART;

static inline void putc(char c)
{
	/* UART enabled? */
	if (!(UART[UART_IER] & UART_IER_UUE))
		return;

	while (!(UART[UART_LSR] & UART_LSR_THRE))
		barrier();

	UART[UART_TX] = c;
}

static inline void flush(void)
{
}

static inline void arch_decomp_setup(void)
{
	/* default to UART2 */
	UART = (unsigned long *)UART2_BASE;

	if (machine_is_avengers_lite())
		UART = (unsigned long *)UART3_BASE;
}


#define arch_decomp_wdog()
