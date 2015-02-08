#ifndef _ASM_IST_H
#define _ASM_IST_H



#include <linux/types.h>

struct ist_info {
	__u32 signature;
	__u32 command;
	__u32 event;
	__u32 perf_level;
};

#ifdef __KERNEL__

extern struct ist_info ist_info;

#endif	/* __KERNEL__ */
#endif	/* _ASM_IST_H */
