

#ifndef _ASM_X86_KVM_X86_EMULATE_H
#define _ASM_X86_KVM_X86_EMULATE_H

#include <asm/desc_defs.h>

struct x86_emulate_ctxt;

/* Access completed successfully: continue emulation as normal. */
#define X86EMUL_CONTINUE        0
/* Access is unhandleable: bail from emulation and return error to caller. */
#define X86EMUL_UNHANDLEABLE    1
/* Terminate emulation but return success to the caller. */
#define X86EMUL_PROPAGATE_FAULT 2 /* propagate a generated fault to guest */
#define X86EMUL_RETRY_INSTR     2 /* retry the instruction for some reason */
#define X86EMUL_CMPXCHG_FAILED  2 /* cmpxchg did not see expected value */
struct x86_emulate_ops {
	/*
	 * read_std: Read bytes of standard (non-emulated/special) memory.
	 *           Used for descriptor reading.
	 *  @addr:  [IN ] Linear address from which to read.
	 *  @val:   [OUT] Value read from memory, zero-extended to 'u_long'.
	 *  @bytes: [IN ] Number of bytes to read from memory.
	 */
	int (*read_std)(unsigned long addr, void *val,
			unsigned int bytes, struct kvm_vcpu *vcpu, u32 *error);

	/*
	 * write_std: Write bytes of standard (non-emulated/special) memory.
	 *            Used for descriptor writing.
	 *  @addr:  [IN ] Linear address to which to write.
	 *  @val:   [OUT] Value write to memory, zero-extended to 'u_long'.
	 *  @bytes: [IN ] Number of bytes to write to memory.
	 */
	int (*write_std)(unsigned long addr, void *val,
			 unsigned int bytes, struct kvm_vcpu *vcpu, u32 *error);
	/*
	 * fetch: Read bytes of standard (non-emulated/special) memory.
	 *        Used for instruction fetch.
	 *  @addr:  [IN ] Linear address from which to read.
	 *  @val:   [OUT] Value read from memory, zero-extended to 'u_long'.
	 *  @bytes: [IN ] Number of bytes to read from memory.
	 */
	int (*fetch)(unsigned long addr, void *val,
			unsigned int bytes, struct kvm_vcpu *vcpu, u32 *error);

	/*
	 * read_emulated: Read bytes from emulated/special memory area.
	 *  @addr:  [IN ] Linear address from which to read.
	 *  @val:   [OUT] Value read from memory, zero-extended to 'u_long'.
	 *  @bytes: [IN ] Number of bytes to read from memory.
	 */
	int (*read_emulated)(unsigned long addr,
			     void *val,
			     unsigned int bytes,
			     struct kvm_vcpu *vcpu);

	/*
	 * write_emulated: Write bytes to emulated/special memory area.
	 *  @addr:  [IN ] Linear address to which to write.
	 *  @val:   [IN ] Value to write to memory (low-order bytes used as
	 *                required).
	 *  @bytes: [IN ] Number of bytes to write to memory.
	 */
	int (*write_emulated)(unsigned long addr,
			      const void *val,
			      unsigned int bytes,
			      struct kvm_vcpu *vcpu);

	/*
	 * cmpxchg_emulated: Emulate an atomic (LOCKed) CMPXCHG operation on an
	 *                   emulated/special memory area.
	 *  @addr:  [IN ] Linear address to access.
	 *  @old:   [IN ] Value expected to be current at @addr.
	 *  @new:   [IN ] Value to write to @addr.
	 *  @bytes: [IN ] Number of bytes to access using CMPXCHG.
	 */
	int (*cmpxchg_emulated)(unsigned long addr,
				const void *old,
				const void *new,
				unsigned int bytes,
				struct kvm_vcpu *vcpu);

	int (*pio_in_emulated)(int size, unsigned short port, void *val,
			       unsigned int count, struct kvm_vcpu *vcpu);

	int (*pio_out_emulated)(int size, unsigned short port, const void *val,
				unsigned int count, struct kvm_vcpu *vcpu);

	bool (*get_cached_descriptor)(struct desc_struct *desc,
				      int seg, struct kvm_vcpu *vcpu);
	void (*set_cached_descriptor)(struct desc_struct *desc,
				      int seg, struct kvm_vcpu *vcpu);
	u16 (*get_segment_selector)(int seg, struct kvm_vcpu *vcpu);
	void (*set_segment_selector)(u16 sel, int seg, struct kvm_vcpu *vcpu);
	void (*get_gdt)(struct desc_ptr *dt, struct kvm_vcpu *vcpu);
	ulong (*get_cr)(int cr, struct kvm_vcpu *vcpu);
	void (*set_cr)(int cr, ulong val, struct kvm_vcpu *vcpu);
	int (*cpl)(struct kvm_vcpu *vcpu);
	void (*set_rflags)(struct kvm_vcpu *vcpu, unsigned long rflags);
};

/* Type, address-of, and value of an instruction's operand. */
struct operand {
	enum { OP_REG, OP_MEM, OP_IMM, OP_NONE } type;
	unsigned int bytes;
	unsigned long val, orig_val, *ptr;
};

struct fetch_cache {
	u8 data[15];
	unsigned long start;
	unsigned long end;
};

struct read_cache {
	u8 data[1024];
	unsigned long pos;
	unsigned long end;
};

struct decode_cache {
	u8 twobyte;
	u8 b;
	u8 lock_prefix;
	u8 rep_prefix;
	u8 op_bytes;
	u8 ad_bytes;
	u8 rex_prefix;
	struct operand src;
	struct operand src2;
	struct operand dst;
	bool has_seg_override;
	u8 seg_override;
	unsigned int d;
	unsigned long regs[NR_VCPU_REGS];
	unsigned long eip;
	/* modrm */
	u8 modrm;
	u8 modrm_mod;
	u8 modrm_reg;
	u8 modrm_rm;
	u8 use_modrm_ea;
	bool rip_relative;
	unsigned long modrm_ea;
	void *modrm_ptr;
	unsigned long modrm_val;
	struct fetch_cache fetch;
	struct read_cache io_read;
};

struct x86_emulate_ctxt {
	/* Register state before/after emulation. */
	struct kvm_vcpu *vcpu;

	unsigned long eflags;
	unsigned long eip; /* eip before instruction emulation */
	/* Emulated execution mode, represented by an X86EMUL_MODE value. */
	int mode;
	u32 cs_base;

	/* interruptibility state, as a result of execution of STI or MOV SS */
	int interruptibility;

	bool restart; /* restart string instruction after writeback */
	/* decode cache */
	struct decode_cache decode;
};

/* Repeat String Operation Prefix */
#define REPE_PREFIX	1
#define REPNE_PREFIX	2

/* Execution mode, passed to the emulator. */
#define X86EMUL_MODE_REAL     0	/* Real mode.             */
#define X86EMUL_MODE_VM86     1	/* Virtual 8086 mode.     */
#define X86EMUL_MODE_PROT16   2	/* 16-bit protected mode. */
#define X86EMUL_MODE_PROT32   4	/* 32-bit protected mode. */
#define X86EMUL_MODE_PROT64   8	/* 64-bit (long) mode.    */

/* Host execution mode. */
#if defined(CONFIG_X86_32)
#define X86EMUL_MODE_HOST X86EMUL_MODE_PROT32
#elif defined(CONFIG_X86_64)
#define X86EMUL_MODE_HOST X86EMUL_MODE_PROT64
#endif

int x86_decode_insn(struct x86_emulate_ctxt *ctxt,
		    struct x86_emulate_ops *ops);
int x86_emulate_insn(struct x86_emulate_ctxt *ctxt,
		     struct x86_emulate_ops *ops);
int emulator_task_switch(struct x86_emulate_ctxt *ctxt,
			 struct x86_emulate_ops *ops,
			 u16 tss_selector, int reason,
			 bool has_error_code, u32 error_code);

#endif /* _ASM_X86_KVM_X86_EMULATE_H */
