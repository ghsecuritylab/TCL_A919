

#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/io.h>

#include <asm/mach/time.h>

#include <mach/regs-timer.h>
#include <mach/regs-irq.h>

#include "generic.h"

static unsigned long ks8695_gettimeoffset (void)
{
	unsigned long elapsed, tick2, intpending;

	/*
	 * Get the current number of ticks.  Note that there is a race
	 * condition between us reading the timer and checking for an
	 * interrupt.  We solve this by ensuring that the counter has not
	 * reloaded between our two reads.
	 */
	elapsed = __raw_readl(KS8695_TMR_VA + KS8695_T1TC) + __raw_readl(KS8695_TMR_VA + KS8695_T1PD);
	do {
		tick2 = elapsed;
		intpending = __raw_readl(KS8695_IRQ_VA + KS8695_INTST) & (1 << KS8695_IRQ_TIMER1);
		elapsed = __raw_readl(KS8695_TMR_VA + KS8695_T1TC) + __raw_readl(KS8695_TMR_VA + KS8695_T1PD);
	} while (elapsed > tick2);

	/* Convert to number of ticks expired (not remaining) */
	elapsed = (CLOCK_TICK_RATE / HZ) - elapsed;

	/* Is interrupt pending?  If so, then timer has been reloaded already. */
	if (intpending)
		elapsed += (CLOCK_TICK_RATE / HZ);

	/* Convert ticks to usecs */
	return (unsigned long)(elapsed * (tick_nsec / 1000)) / LATCH;
}

static irqreturn_t ks8695_timer_interrupt(int irq, void *dev_id)
{
	timer_tick();
	return IRQ_HANDLED;
}

static struct irqaction ks8695_timer_irq = {
	.name		= "ks8695_tick",
	.flags		= IRQF_DISABLED | IRQF_TIMER,
	.handler	= ks8695_timer_interrupt,
};

static void ks8695_timer_setup(void)
{
	unsigned long tmout = CLOCK_TICK_RATE / HZ;
	unsigned long tmcon;

	/* disable timer1 */
	tmcon = __raw_readl(KS8695_TMR_VA + KS8695_TMCON);
	__raw_writel(tmcon & ~TMCON_T1EN, KS8695_TMR_VA + KS8695_TMCON);

	__raw_writel(tmout / 2, KS8695_TMR_VA + KS8695_T1TC);
	__raw_writel(tmout / 2, KS8695_TMR_VA + KS8695_T1PD);

	/* re-enable timer1 */
	__raw_writel(tmcon | TMCON_T1EN, KS8695_TMR_VA + KS8695_TMCON);
}

static void __init ks8695_timer_init (void)
{
	ks8695_timer_setup();

	/* Enable timer interrupts */
	setup_irq(KS8695_IRQ_TIMER1, &ks8695_timer_irq);
}

struct sys_timer ks8695_timer = {
	.init		= ks8695_timer_init,
	.offset		= ks8695_gettimeoffset,
	.resume		= ks8695_timer_setup,
};
