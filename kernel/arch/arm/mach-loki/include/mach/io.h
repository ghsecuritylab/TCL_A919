

#ifndef __ASM_ARCH_IO_H
#define __ASM_ARCH_IO_H

#include "loki.h"

#define IO_SPACE_LIMIT		0xffffffff

static inline void __iomem *__io(unsigned long addr)
{
	return (void __iomem *)((addr - LOKI_PCIE0_IO_PHYS_BASE)
					+ LOKI_PCIE0_IO_VIRT_BASE);
}

#define __io(a)			__io(a)
#define __mem_pci(a)		(a)


#endif
