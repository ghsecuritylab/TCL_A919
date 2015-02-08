


#ifndef __ARCH_ARM_MACH_OMAP2_CLOCKDOMAINS44XX_H
#define __ARCH_ARM_MACH_OMAP2_CLOCKDOMAINS44XX_H

#include <plat/clockdomain.h>

#if defined(CONFIG_ARCH_OMAP4)

static struct clockdomain l4_cefuse_44xx_clkdm = {
	.name		  = "l4_cefuse_clkdm",
	.pwrdm		  = { .name = "cefuse_pwrdm" },
	.clkstctrl_reg	  = OMAP4430_CM_CEFUSE_CLKSTCTRL,
	.clktrctrl_mask	  = OMAP4430_CLKTRCTRL_MASK,
	.flags		  = CLKDM_CAN_FORCE_WAKEUP | CLKDM_CAN_HWSUP,
	.omap_chip	  = OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

static struct clockdomain l4_cfg_44xx_clkdm = {
	.name		  = "l4_cfg_clkdm",
	.pwrdm		  = { .name = "core_pwrdm" },
	.clkstctrl_reg	  = OMAP4430_CM_L4CFG_CLKSTCTRL,
	.clktrctrl_mask	  = OMAP4430_CLKTRCTRL_MASK,
	.flags		  = CLKDM_CAN_HWSUP,
	.omap_chip	  = OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

static struct clockdomain tesla_44xx_clkdm = {
	.name		  = "tesla_clkdm",
	.pwrdm		  = { .name = "tesla_pwrdm" },
	.clkstctrl_reg	  = OMAP4430_CM_TESLA_CLKSTCTRL,
	.clktrctrl_mask	  = OMAP4430_CLKTRCTRL_MASK,
	.flags		  = CLKDM_CAN_HWSUP_SWSUP,
	.omap_chip	  = OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

static struct clockdomain l3_gfx_44xx_clkdm = {
	.name		  = "l3_gfx_clkdm",
	.pwrdm		  = { .name = "gfx_pwrdm" },
	.clkstctrl_reg	  = OMAP4430_CM_GFX_CLKSTCTRL,
	.clktrctrl_mask	  = OMAP4430_CLKTRCTRL_MASK,
	.flags		  = CLKDM_CAN_HWSUP_SWSUP,
	.omap_chip	  = OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

static struct clockdomain ivahd_44xx_clkdm = {
	.name		  = "ivahd_clkdm",
	.pwrdm		  = { .name = "ivahd_pwrdm" },
	.clkstctrl_reg	  = OMAP4430_CM_IVAHD_CLKSTCTRL,
	.clktrctrl_mask	  = OMAP4430_CLKTRCTRL_MASK,
	.flags		  = CLKDM_CAN_HWSUP_SWSUP,
	.omap_chip	  = OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

static struct clockdomain l4_secure_44xx_clkdm = {
	.name		  = "l4_secure_clkdm",
	.pwrdm		  = { .name = "l4per_pwrdm" },
	.clkstctrl_reg	  = OMAP4430_CM_L4SEC_CLKSTCTRL,
	.clktrctrl_mask	  = OMAP4430_CLKTRCTRL_MASK,
	.flags		  = CLKDM_CAN_HWSUP_SWSUP,
	.omap_chip	  = OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

static struct clockdomain l4_per_44xx_clkdm = {
	.name		  = "l4_per_clkdm",
	.pwrdm		  = { .name = "l4per_pwrdm" },
	.clkstctrl_reg	  = OMAP4430_CM_L4PER_CLKSTCTRL,
	.clktrctrl_mask	  = OMAP4430_CLKTRCTRL_MASK,
	.flags		  = CLKDM_CAN_HWSUP_SWSUP,
	.omap_chip	  = OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

static struct clockdomain abe_44xx_clkdm = {
	.name		  = "abe_clkdm",
	.pwrdm		  = { .name = "abe_pwrdm" },
	.clkstctrl_reg	  = OMAP4430_CM1_ABE_CLKSTCTRL,
	.clktrctrl_mask	  = OMAP4430_CLKTRCTRL_MASK,
	.flags		  = CLKDM_CAN_HWSUP_SWSUP,
	.omap_chip	  = OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

static struct clockdomain l3_instr_44xx_clkdm = {
	.name		  = "l3_instr_clkdm",
	.pwrdm		  = { .name = "core_pwrdm" },
	.clkstctrl_reg	  = OMAP4430_CM_L3INSTR_CLKSTCTRL,
	.clktrctrl_mask	  = OMAP4430_CLKTRCTRL_MASK,
	.omap_chip	  = OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

static struct clockdomain l3_init_44xx_clkdm = {
	.name		  = "l3_init_clkdm",
	.pwrdm		  = { .name = "l3init_pwrdm" },
	.clkstctrl_reg	  = OMAP4430_CM_L3INIT_CLKSTCTRL,
	.clktrctrl_mask	  = OMAP4430_CLKTRCTRL_MASK,
	.flags		  = CLKDM_CAN_HWSUP_SWSUP,
	.omap_chip	  = OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

static struct clockdomain mpuss_44xx_clkdm = {
	.name		  = "mpuss_clkdm",
	.pwrdm		  = { .name = "mpu_pwrdm" },
	.clkstctrl_reg	  = OMAP4430_CM_MPU_CLKSTCTRL,
	.clktrctrl_mask	  = OMAP4430_CLKTRCTRL_MASK,
	.flags		  = CLKDM_CAN_FORCE_WAKEUP | CLKDM_CAN_HWSUP,
	.omap_chip	  = OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

static struct clockdomain mpu0_44xx_clkdm = {
	.name		  = "mpu0_clkdm",
	.pwrdm		  = { .name = "cpu0_pwrdm" },
	.clkstctrl_reg	  = OMAP4430_CM_CPU0_CLKSTCTRL,
	.clktrctrl_mask	  = OMAP4430_CLKTRCTRL_MASK,
	.flags		  = CLKDM_CAN_FORCE_WAKEUP | CLKDM_CAN_HWSUP,
	.omap_chip	  = OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

static struct clockdomain mpu1_44xx_clkdm = {
	.name		  = "mpu1_clkdm",
	.pwrdm		  = { .name = "cpu1_pwrdm" },
	.clkstctrl_reg	  = OMAP4430_CM_CPU1_CLKSTCTRL,
	.clktrctrl_mask	  = OMAP4430_CLKTRCTRL_MASK,
	.flags		  = CLKDM_CAN_FORCE_WAKEUP | CLKDM_CAN_HWSUP,
	.omap_chip	  = OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

static struct clockdomain l3_emif_44xx_clkdm = {
	.name		  = "l3_emif_clkdm",
	.pwrdm		  = { .name = "core_pwrdm" },
	.clkstctrl_reg	  = OMAP4430_CM_MEMIF_CLKSTCTRL,
	.clktrctrl_mask	  = OMAP4430_CLKTRCTRL_MASK,
	.flags		  = CLKDM_CAN_FORCE_WAKEUP | CLKDM_CAN_HWSUP,
	.omap_chip	  = OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

static struct clockdomain l4_ao_44xx_clkdm = {
	.name		  = "l4_ao_clkdm",
	.pwrdm		  = { .name = "always_on_core_pwrdm" },
	.clkstctrl_reg	  = OMAP4430_CM_ALWON_CLKSTCTRL,
	.clktrctrl_mask	  = OMAP4430_CLKTRCTRL_MASK,
	.flags		  = CLKDM_CAN_FORCE_WAKEUP | CLKDM_CAN_HWSUP,
	.omap_chip	  = OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

static struct clockdomain ducati_44xx_clkdm = {
	.name		  = "ducati_clkdm",
	.pwrdm		  = { .name = "core_pwrdm" },
	.clkstctrl_reg	  = OMAP4430_CM_DUCATI_CLKSTCTRL,
	.clktrctrl_mask	  = OMAP4430_CLKTRCTRL_MASK,
	.flags		  = CLKDM_CAN_HWSUP_SWSUP,
	.omap_chip	  = OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

static struct clockdomain l3_2_44xx_clkdm = {
	.name		  = "l3_2_clkdm",
	.pwrdm		  = { .name = "core_pwrdm" },
	.clkstctrl_reg	  = OMAP4430_CM_L3_2_CLKSTCTRL,
	.clktrctrl_mask	  = OMAP4430_CLKTRCTRL_MASK,
	.flags		  = CLKDM_CAN_HWSUP,
	.omap_chip	  = OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

static struct clockdomain l3_1_44xx_clkdm = {
	.name		  = "l3_1_clkdm",
	.pwrdm		  = { .name = "core_pwrdm" },
	.clkstctrl_reg	  = OMAP4430_CM_L3_1_CLKSTCTRL,
	.clktrctrl_mask	  = OMAP4430_CLKTRCTRL_MASK,
	.flags		  = CLKDM_CAN_HWSUP,
	.omap_chip	  = OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

static struct clockdomain l3_d2d_44xx_clkdm = {
	.name		  = "l3_d2d_clkdm",
	.pwrdm		  = { .name = "core_pwrdm" },
	.clkstctrl_reg	  = OMAP4430_CM_D2D_CLKSTCTRL,
	.clktrctrl_mask	  = OMAP4430_CLKTRCTRL_MASK,
	.flags		  = CLKDM_CAN_FORCE_WAKEUP | CLKDM_CAN_HWSUP,
	.omap_chip	  = OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

static struct clockdomain iss_44xx_clkdm = {
	.name		  = "iss_clkdm",
	.pwrdm		  = { .name = "cam_pwrdm" },
	.clkstctrl_reg	  = OMAP4430_CM_CAM_CLKSTCTRL,
	.clktrctrl_mask	  = OMAP4430_CLKTRCTRL_MASK,
	.flags		  = CLKDM_CAN_HWSUP_SWSUP,
	.omap_chip	  = OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

static struct clockdomain l3_dss_44xx_clkdm = {
	.name		  = "l3_dss_clkdm",
	.pwrdm		  = { .name = "dss_pwrdm" },
	.clkstctrl_reg	  = OMAP4430_CM_DSS_CLKSTCTRL,
	.clktrctrl_mask	  = OMAP4430_CLKTRCTRL_MASK,
	.flags		  = CLKDM_CAN_HWSUP_SWSUP,
	.omap_chip	  = OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

static struct clockdomain l4_wkup_44xx_clkdm = {
	.name		  = "l4_wkup_clkdm",
	.pwrdm		  = { .name = "wkup_pwrdm" },
	.clkstctrl_reg	  = OMAP4430_CM_WKUP_CLKSTCTRL,
	.clktrctrl_mask	  = OMAP4430_CLKTRCTRL_MASK,
	.flags		  = CLKDM_CAN_HWSUP,
	.omap_chip	  = OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

static struct clockdomain emu_sys_44xx_clkdm = {
	.name		  = "emu_sys_clkdm",
	.pwrdm		  = { .name = "emu_pwrdm" },
	.clkstctrl_reg	  = OMAP4430_CM_EMU_CLKSTCTRL,
	.clktrctrl_mask	  = OMAP4430_CLKTRCTRL_MASK,
	.flags		  = CLKDM_CAN_HWSUP,
	.omap_chip	  = OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

static struct clockdomain l3_dma_44xx_clkdm = {
	.name		  = "l3_dma_clkdm",
	.pwrdm		  = { .name = "core_pwrdm" },
	.clkstctrl_reg	  = OMAP4430_CM_SDMA_CLKSTCTRL,
	.clktrctrl_mask	  = OMAP4430_CLKTRCTRL_MASK,
	.flags		  = CLKDM_CAN_FORCE_WAKEUP | CLKDM_CAN_HWSUP,
	.omap_chip	  = OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

#endif

#endif
