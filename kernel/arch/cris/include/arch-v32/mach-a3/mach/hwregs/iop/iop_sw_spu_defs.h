
#ifndef __iop_sw_spu_defs_h
#define __iop_sw_spu_defs_h

/* Main access macros */
#ifndef REG_RD
#define REG_RD( scope, inst, reg ) \
  REG_READ( reg_##scope##_##reg, \
            (inst) + REG_RD_ADDR_##scope##_##reg )
#endif

#ifndef REG_WR
#define REG_WR( scope, inst, reg, val ) \
  REG_WRITE( reg_##scope##_##reg, \
             (inst) + REG_WR_ADDR_##scope##_##reg, (val) )
#endif

#ifndef REG_RD_VECT
#define REG_RD_VECT( scope, inst, reg, index ) \
  REG_READ( reg_##scope##_##reg, \
            (inst) + REG_RD_ADDR_##scope##_##reg + \
	    (index) * STRIDE_##scope##_##reg )
#endif

#ifndef REG_WR_VECT
#define REG_WR_VECT( scope, inst, reg, index, val ) \
  REG_WRITE( reg_##scope##_##reg, \
             (inst) + REG_WR_ADDR_##scope##_##reg + \
	     (index) * STRIDE_##scope##_##reg, (val) )
#endif

#ifndef REG_RD_INT
#define REG_RD_INT( scope, inst, reg ) \
  REG_READ( int, (inst) + REG_RD_ADDR_##scope##_##reg )
#endif

#ifndef REG_WR_INT
#define REG_WR_INT( scope, inst, reg, val ) \
  REG_WRITE( int, (inst) + REG_WR_ADDR_##scope##_##reg, (val) )
#endif

#ifndef REG_RD_INT_VECT
#define REG_RD_INT_VECT( scope, inst, reg, index ) \
  REG_READ( int, (inst) + REG_RD_ADDR_##scope##_##reg + \
	    (index) * STRIDE_##scope##_##reg )
#endif

#ifndef REG_WR_INT_VECT
#define REG_WR_INT_VECT( scope, inst, reg, index, val ) \
  REG_WRITE( int, (inst) + REG_WR_ADDR_##scope##_##reg + \
	     (index) * STRIDE_##scope##_##reg, (val) )
#endif

#ifndef REG_TYPE_CONV
#define REG_TYPE_CONV( type, orgtype, val ) \
  ( { union { orgtype o; type n; } r; r.o = val; r.n; } )
#endif

#ifndef reg_page_size
#define reg_page_size 8192
#endif

#ifndef REG_ADDR
#define REG_ADDR( scope, inst, reg ) \
  ( (inst) + REG_RD_ADDR_##scope##_##reg )
#endif

#ifndef REG_ADDR_VECT
#define REG_ADDR_VECT( scope, inst, reg, index ) \
  ( (inst) + REG_RD_ADDR_##scope##_##reg + \
    (index) * STRIDE_##scope##_##reg )
#endif

/* C-code for register scope iop_sw_spu */

/* Register r_mpu_trace, scope iop_sw_spu, type r */
typedef unsigned int reg_iop_sw_spu_r_mpu_trace;
#define REG_RD_ADDR_iop_sw_spu_r_mpu_trace 0

/* Register rw_mc_ctrl, scope iop_sw_spu, type rw */
typedef struct {
  unsigned int keep_owner : 1;
  unsigned int cmd        : 2;
  unsigned int size       : 3;
  unsigned int wr_spu_mem : 1;
  unsigned int dummy1     : 25;
} reg_iop_sw_spu_rw_mc_ctrl;
#define REG_RD_ADDR_iop_sw_spu_rw_mc_ctrl 4
#define REG_WR_ADDR_iop_sw_spu_rw_mc_ctrl 4

/* Register rw_mc_data, scope iop_sw_spu, type rw */
typedef struct {
  unsigned int val : 32;
} reg_iop_sw_spu_rw_mc_data;
#define REG_RD_ADDR_iop_sw_spu_rw_mc_data 8
#define REG_WR_ADDR_iop_sw_spu_rw_mc_data 8

/* Register rw_mc_addr, scope iop_sw_spu, type rw */
typedef unsigned int reg_iop_sw_spu_rw_mc_addr;
#define REG_RD_ADDR_iop_sw_spu_rw_mc_addr 12
#define REG_WR_ADDR_iop_sw_spu_rw_mc_addr 12

/* Register rs_mc_data, scope iop_sw_spu, type rs */
typedef unsigned int reg_iop_sw_spu_rs_mc_data;
#define REG_RD_ADDR_iop_sw_spu_rs_mc_data 16

/* Register r_mc_data, scope iop_sw_spu, type r */
typedef unsigned int reg_iop_sw_spu_r_mc_data;
#define REG_RD_ADDR_iop_sw_spu_r_mc_data 20

/* Register r_mc_stat, scope iop_sw_spu, type r */
typedef struct {
  unsigned int busy_cpu     : 1;
  unsigned int busy_mpu     : 1;
  unsigned int busy_spu     : 1;
  unsigned int owned_by_cpu : 1;
  unsigned int owned_by_mpu : 1;
  unsigned int owned_by_spu : 1;
  unsigned int dummy1       : 26;
} reg_iop_sw_spu_r_mc_stat;
#define REG_RD_ADDR_iop_sw_spu_r_mc_stat 24

/* Register rw_bus_clr_mask, scope iop_sw_spu, type rw */
typedef struct {
  unsigned int byte0 : 8;
  unsigned int byte1 : 8;
  unsigned int byte2 : 8;
  unsigned int byte3 : 8;
} reg_iop_sw_spu_rw_bus_clr_mask;
#define REG_RD_ADDR_iop_sw_spu_rw_bus_clr_mask 28
#define REG_WR_ADDR_iop_sw_spu_rw_bus_clr_mask 28

/* Register rw_bus_set_mask, scope iop_sw_spu, type rw */
typedef struct {
  unsigned int byte0 : 8;
  unsigned int byte1 : 8;
  unsigned int byte2 : 8;
  unsigned int byte3 : 8;
} reg_iop_sw_spu_rw_bus_set_mask;
#define REG_RD_ADDR_iop_sw_spu_rw_bus_set_mask 32
#define REG_WR_ADDR_iop_sw_spu_rw_bus_set_mask 32

/* Register rw_bus_oe_clr_mask, scope iop_sw_spu, type rw */
typedef struct {
  unsigned int byte0 : 1;
  unsigned int byte1 : 1;
  unsigned int byte2 : 1;
  unsigned int byte3 : 1;
  unsigned int dummy1 : 28;
} reg_iop_sw_spu_rw_bus_oe_clr_mask;
#define REG_RD_ADDR_iop_sw_spu_rw_bus_oe_clr_mask 36
#define REG_WR_ADDR_iop_sw_spu_rw_bus_oe_clr_mask 36

/* Register rw_bus_oe_set_mask, scope iop_sw_spu, type rw */
typedef struct {
  unsigned int byte0 : 1;
  unsigned int byte1 : 1;
  unsigned int byte2 : 1;
  unsigned int byte3 : 1;
  unsigned int dummy1 : 28;
} reg_iop_sw_spu_rw_bus_oe_set_mask;
#define REG_RD_ADDR_iop_sw_spu_rw_bus_oe_set_mask 40
#define REG_WR_ADDR_iop_sw_spu_rw_bus_oe_set_mask 40

/* Register r_bus_in, scope iop_sw_spu, type r */
typedef unsigned int reg_iop_sw_spu_r_bus_in;
#define REG_RD_ADDR_iop_sw_spu_r_bus_in 44

/* Register rw_gio_clr_mask, scope iop_sw_spu, type rw */
typedef struct {
  unsigned int val : 32;
} reg_iop_sw_spu_rw_gio_clr_mask;
#define REG_RD_ADDR_iop_sw_spu_rw_gio_clr_mask 48
#define REG_WR_ADDR_iop_sw_spu_rw_gio_clr_mask 48

/* Register rw_gio_set_mask, scope iop_sw_spu, type rw */
typedef struct {
  unsigned int val : 32;
} reg_iop_sw_spu_rw_gio_set_mask;
#define REG_RD_ADDR_iop_sw_spu_rw_gio_set_mask 52
#define REG_WR_ADDR_iop_sw_spu_rw_gio_set_mask 52

/* Register rw_gio_oe_clr_mask, scope iop_sw_spu, type rw */
typedef struct {
  unsigned int val : 32;
} reg_iop_sw_spu_rw_gio_oe_clr_mask;
#define REG_RD_ADDR_iop_sw_spu_rw_gio_oe_clr_mask 56
#define REG_WR_ADDR_iop_sw_spu_rw_gio_oe_clr_mask 56

/* Register rw_gio_oe_set_mask, scope iop_sw_spu, type rw */
typedef struct {
  unsigned int val : 32;
} reg_iop_sw_spu_rw_gio_oe_set_mask;
#define REG_RD_ADDR_iop_sw_spu_rw_gio_oe_set_mask 60
#define REG_WR_ADDR_iop_sw_spu_rw_gio_oe_set_mask 60

/* Register r_gio_in, scope iop_sw_spu, type r */
typedef unsigned int reg_iop_sw_spu_r_gio_in;
#define REG_RD_ADDR_iop_sw_spu_r_gio_in 64

/* Register rw_bus_clr_mask_lo, scope iop_sw_spu, type rw */
typedef struct {
  unsigned int byte0 : 8;
  unsigned int byte1 : 8;
  unsigned int dummy1 : 16;
} reg_iop_sw_spu_rw_bus_clr_mask_lo;
#define REG_RD_ADDR_iop_sw_spu_rw_bus_clr_mask_lo 68
#define REG_WR_ADDR_iop_sw_spu_rw_bus_clr_mask_lo 68

/* Register rw_bus_clr_mask_hi, scope iop_sw_spu, type rw */
typedef struct {
  unsigned int byte2 : 8;
  unsigned int byte3 : 8;
  unsigned int dummy1 : 16;
} reg_iop_sw_spu_rw_bus_clr_mask_hi;
#define REG_RD_ADDR_iop_sw_spu_rw_bus_clr_mask_hi 72
#define REG_WR_ADDR_iop_sw_spu_rw_bus_clr_mask_hi 72

/* Register rw_bus_set_mask_lo, scope iop_sw_spu, type rw */
typedef struct {
  unsigned int byte0 : 8;
  unsigned int byte1 : 8;
  unsigned int dummy1 : 16;
} reg_iop_sw_spu_rw_bus_set_mask_lo;
#define REG_RD_ADDR_iop_sw_spu_rw_bus_set_mask_lo 76
#define REG_WR_ADDR_iop_sw_spu_rw_bus_set_mask_lo 76

/* Register rw_bus_set_mask_hi, scope iop_sw_spu, type rw */
typedef struct {
  unsigned int byte2 : 8;
  unsigned int byte3 : 8;
  unsigned int dummy1 : 16;
} reg_iop_sw_spu_rw_bus_set_mask_hi;
#define REG_RD_ADDR_iop_sw_spu_rw_bus_set_mask_hi 80
#define REG_WR_ADDR_iop_sw_spu_rw_bus_set_mask_hi 80

/* Register rw_gio_clr_mask_lo, scope iop_sw_spu, type rw */
typedef struct {
  unsigned int val : 16;
  unsigned int dummy1 : 16;
} reg_iop_sw_spu_rw_gio_clr_mask_lo;
#define REG_RD_ADDR_iop_sw_spu_rw_gio_clr_mask_lo 84
#define REG_WR_ADDR_iop_sw_spu_rw_gio_clr_mask_lo 84

/* Register rw_gio_clr_mask_hi, scope iop_sw_spu, type rw */
typedef struct {
  unsigned int val : 16;
  unsigned int dummy1 : 16;
} reg_iop_sw_spu_rw_gio_clr_mask_hi;
#define REG_RD_ADDR_iop_sw_spu_rw_gio_clr_mask_hi 88
#define REG_WR_ADDR_iop_sw_spu_rw_gio_clr_mask_hi 88

/* Register rw_gio_set_mask_lo, scope iop_sw_spu, type rw */
typedef struct {
  unsigned int val : 16;
  unsigned int dummy1 : 16;
} reg_iop_sw_spu_rw_gio_set_mask_lo;
#define REG_RD_ADDR_iop_sw_spu_rw_gio_set_mask_lo 92
#define REG_WR_ADDR_iop_sw_spu_rw_gio_set_mask_lo 92

/* Register rw_gio_set_mask_hi, scope iop_sw_spu, type rw */
typedef struct {
  unsigned int val : 16;
  unsigned int dummy1 : 16;
} reg_iop_sw_spu_rw_gio_set_mask_hi;
#define REG_RD_ADDR_iop_sw_spu_rw_gio_set_mask_hi 96
#define REG_WR_ADDR_iop_sw_spu_rw_gio_set_mask_hi 96

/* Register rw_gio_oe_clr_mask_lo, scope iop_sw_spu, type rw */
typedef struct {
  unsigned int val : 16;
  unsigned int dummy1 : 16;
} reg_iop_sw_spu_rw_gio_oe_clr_mask_lo;
#define REG_RD_ADDR_iop_sw_spu_rw_gio_oe_clr_mask_lo 100
#define REG_WR_ADDR_iop_sw_spu_rw_gio_oe_clr_mask_lo 100

/* Register rw_gio_oe_clr_mask_hi, scope iop_sw_spu, type rw */
typedef struct {
  unsigned int val : 16;
  unsigned int dummy1 : 16;
} reg_iop_sw_spu_rw_gio_oe_clr_mask_hi;
#define REG_RD_ADDR_iop_sw_spu_rw_gio_oe_clr_mask_hi 104
#define REG_WR_ADDR_iop_sw_spu_rw_gio_oe_clr_mask_hi 104

/* Register rw_gio_oe_set_mask_lo, scope iop_sw_spu, type rw */
typedef struct {
  unsigned int val : 16;
  unsigned int dummy1 : 16;
} reg_iop_sw_spu_rw_gio_oe_set_mask_lo;
#define REG_RD_ADDR_iop_sw_spu_rw_gio_oe_set_mask_lo 108
#define REG_WR_ADDR_iop_sw_spu_rw_gio_oe_set_mask_lo 108

/* Register rw_gio_oe_set_mask_hi, scope iop_sw_spu, type rw */
typedef struct {
  unsigned int val : 16;
  unsigned int dummy1 : 16;
} reg_iop_sw_spu_rw_gio_oe_set_mask_hi;
#define REG_RD_ADDR_iop_sw_spu_rw_gio_oe_set_mask_hi 112
#define REG_WR_ADDR_iop_sw_spu_rw_gio_oe_set_mask_hi 112

/* Register rw_cpu_intr, scope iop_sw_spu, type rw */
typedef struct {
  unsigned int intr0  : 1;
  unsigned int intr1  : 1;
  unsigned int intr2  : 1;
  unsigned int intr3  : 1;
  unsigned int intr4  : 1;
  unsigned int intr5  : 1;
  unsigned int intr6  : 1;
  unsigned int intr7  : 1;
  unsigned int intr8  : 1;
  unsigned int intr9  : 1;
  unsigned int intr10 : 1;
  unsigned int intr11 : 1;
  unsigned int intr12 : 1;
  unsigned int intr13 : 1;
  unsigned int intr14 : 1;
  unsigned int intr15 : 1;
  unsigned int dummy1 : 16;
} reg_iop_sw_spu_rw_cpu_intr;
#define REG_RD_ADDR_iop_sw_spu_rw_cpu_intr 116
#define REG_WR_ADDR_iop_sw_spu_rw_cpu_intr 116

/* Register r_cpu_intr, scope iop_sw_spu, type r */
typedef struct {
  unsigned int intr0  : 1;
  unsigned int intr1  : 1;
  unsigned int intr2  : 1;
  unsigned int intr3  : 1;
  unsigned int intr4  : 1;
  unsigned int intr5  : 1;
  unsigned int intr6  : 1;
  unsigned int intr7  : 1;
  unsigned int intr8  : 1;
  unsigned int intr9  : 1;
  unsigned int intr10 : 1;
  unsigned int intr11 : 1;
  unsigned int intr12 : 1;
  unsigned int intr13 : 1;
  unsigned int intr14 : 1;
  unsigned int intr15 : 1;
  unsigned int dummy1 : 16;
} reg_iop_sw_spu_r_cpu_intr;
#define REG_RD_ADDR_iop_sw_spu_r_cpu_intr 120

/* Register r_hw_intr, scope iop_sw_spu, type r */
typedef struct {
  unsigned int trigger_grp0   : 1;
  unsigned int trigger_grp1   : 1;
  unsigned int trigger_grp2   : 1;
  unsigned int trigger_grp3   : 1;
  unsigned int trigger_grp4   : 1;
  unsigned int trigger_grp5   : 1;
  unsigned int trigger_grp6   : 1;
  unsigned int trigger_grp7   : 1;
  unsigned int timer_grp0     : 1;
  unsigned int timer_grp1     : 1;
  unsigned int fifo_out       : 1;
  unsigned int fifo_out_extra : 1;
  unsigned int fifo_in        : 1;
  unsigned int fifo_in_extra  : 1;
  unsigned int dmc_out        : 1;
  unsigned int dmc_in         : 1;
  unsigned int dummy1         : 16;
} reg_iop_sw_spu_r_hw_intr;
#define REG_RD_ADDR_iop_sw_spu_r_hw_intr 124

/* Register rw_mpu_intr, scope iop_sw_spu, type rw */
typedef struct {
  unsigned int intr0  : 1;
  unsigned int intr1  : 1;
  unsigned int intr2  : 1;
  unsigned int intr3  : 1;
  unsigned int intr4  : 1;
  unsigned int intr5  : 1;
  unsigned int intr6  : 1;
  unsigned int intr7  : 1;
  unsigned int intr8  : 1;
  unsigned int intr9  : 1;
  unsigned int intr10 : 1;
  unsigned int intr11 : 1;
  unsigned int intr12 : 1;
  unsigned int intr13 : 1;
  unsigned int intr14 : 1;
  unsigned int intr15 : 1;
  unsigned int dummy1 : 16;
} reg_iop_sw_spu_rw_mpu_intr;
#define REG_RD_ADDR_iop_sw_spu_rw_mpu_intr 128
#define REG_WR_ADDR_iop_sw_spu_rw_mpu_intr 128

/* Register r_mpu_intr, scope iop_sw_spu, type r */
typedef struct {
  unsigned int intr0  : 1;
  unsigned int intr1  : 1;
  unsigned int intr2  : 1;
  unsigned int intr3  : 1;
  unsigned int intr4  : 1;
  unsigned int intr5  : 1;
  unsigned int intr6  : 1;
  unsigned int intr7  : 1;
  unsigned int intr8  : 1;
  unsigned int intr9  : 1;
  unsigned int intr10 : 1;
  unsigned int intr11 : 1;
  unsigned int intr12 : 1;
  unsigned int intr13 : 1;
  unsigned int intr14 : 1;
  unsigned int intr15 : 1;
  unsigned int dummy1 : 16;
} reg_iop_sw_spu_r_mpu_intr;
#define REG_RD_ADDR_iop_sw_spu_r_mpu_intr 132


/* Constants */
enum {
  regk_iop_sw_spu_copy                     = 0x00000000,
  regk_iop_sw_spu_no                       = 0x00000000,
  regk_iop_sw_spu_nop                      = 0x00000000,
  regk_iop_sw_spu_rd                       = 0x00000002,
  regk_iop_sw_spu_reg_copy                 = 0x00000001,
  regk_iop_sw_spu_rw_bus_clr_mask_default  = 0x00000000,
  regk_iop_sw_spu_rw_bus_oe_clr_mask_default = 0x00000000,
  regk_iop_sw_spu_rw_bus_oe_set_mask_default = 0x00000000,
  regk_iop_sw_spu_rw_bus_set_mask_default  = 0x00000000,
  regk_iop_sw_spu_rw_gio_clr_mask_default  = 0x00000000,
  regk_iop_sw_spu_rw_gio_oe_clr_mask_default = 0x00000000,
  regk_iop_sw_spu_rw_gio_oe_set_mask_default = 0x00000000,
  regk_iop_sw_spu_rw_gio_set_mask_default  = 0x00000000,
  regk_iop_sw_spu_set                      = 0x00000001,
  regk_iop_sw_spu_wr                       = 0x00000003,
  regk_iop_sw_spu_yes                      = 0x00000001
};
#endif /* __iop_sw_spu_defs_h */
