
#ifndef STE_DMA40_DB8500_H
#define STE_DMA40_DB8500_H

#define STEDMA40_NR_DEV 64

enum dma_src_dev_type {
	STEDMA40_DEV_SPI0_RX = 0,
	STEDMA40_DEV_SD_MMC0_RX = 1,
	STEDMA40_DEV_SD_MMC1_RX = 2,
	STEDMA40_DEV_SD_MMC2_RX = 3,
	STEDMA40_DEV_I2C1_RX = 4,
	STEDMA40_DEV_I2C3_RX = 5,
	STEDMA40_DEV_I2C2_RX = 6,
	STEDMA40_DEV_I2C4_RX = 7, /* Only on V1 */
	STEDMA40_DEV_SSP0_RX = 8,
	STEDMA40_DEV_SSP1_RX = 9,
	STEDMA40_DEV_MCDE_RX = 10,
	STEDMA40_DEV_UART2_RX = 11,
	STEDMA40_DEV_UART1_RX = 12,
	STEDMA40_DEV_UART0_RX = 13,
	STEDMA40_DEV_MSP2_RX = 14,
	STEDMA40_DEV_I2C0_RX = 15,
	STEDMA40_DEV_USB_OTG_IEP_8 = 16,
	STEDMA40_DEV_USB_OTG_IEP_1_9 = 17,
	STEDMA40_DEV_USB_OTG_IEP_2_10 = 18,
	STEDMA40_DEV_USB_OTG_IEP_3_11 = 19,
	STEDMA40_DEV_SLIM0_CH0_RX_HSI_RX_CH0 = 20,
	STEDMA40_DEV_SLIM0_CH1_RX_HSI_RX_CH1 = 21,
	STEDMA40_DEV_SLIM0_CH2_RX_HSI_RX_CH2 = 22,
	STEDMA40_DEV_SLIM0_CH3_RX_HSI_RX_CH3 = 23,
	STEDMA40_DEV_SRC_SXA0_RX_TX = 24,
	STEDMA40_DEV_SRC_SXA1_RX_TX = 25,
	STEDMA40_DEV_SRC_SXA2_RX_TX = 26,
	STEDMA40_DEV_SRC_SXA3_RX_TX = 27,
	STEDMA40_DEV_SD_MM2_RX = 28,
	STEDMA40_DEV_SD_MM0_RX = 29,
	STEDMA40_DEV_MSP1_RX = 30,
	/*
	 * This channel is either SlimBus or MSP,
	 * never both at the same time.
	 */
	STEDMA40_SLIM0_CH0_RX = 31,
	STEDMA40_DEV_MSP0_RX = 31,
	STEDMA40_DEV_SD_MM1_RX = 32,
	STEDMA40_DEV_SPI2_RX = 33,
	STEDMA40_DEV_I2C3_RX2 = 34,
	STEDMA40_DEV_SPI1_RX = 35,
	STEDMA40_DEV_USB_OTG_IEP_4_12 = 36,
	STEDMA40_DEV_USB_OTG_IEP_5_13 = 37,
	STEDMA40_DEV_USB_OTG_IEP_6_14 = 38,
	STEDMA40_DEV_USB_OTG_IEP_7_15 = 39,
	STEDMA40_DEV_SPI3_RX = 40,
	STEDMA40_DEV_SD_MM3_RX = 41,
	STEDMA40_DEV_SD_MM4_RX = 42,
	STEDMA40_DEV_SD_MM5_RX = 43,
	STEDMA40_DEV_SRC_SXA4_RX_TX = 44,
	STEDMA40_DEV_SRC_SXA5_RX_TX = 45,
	STEDMA40_DEV_SRC_SXA6_RX_TX = 46,
	STEDMA40_DEV_SRC_SXA7_RX_TX = 47,
	STEDMA40_DEV_CAC1_RX = 48,
	/* RX channels 49 and 50 are unused */
	STEDMA40_DEV_MSHC_RX = 51,
	STEDMA40_DEV_SLIM1_CH0_RX_HSI_RX_CH4 = 52,
	STEDMA40_DEV_SLIM1_CH1_RX_HSI_RX_CH5 = 53,
	STEDMA40_DEV_SLIM1_CH2_RX_HSI_RX_CH6 = 54,
	STEDMA40_DEV_SLIM1_CH3_RX_HSI_RX_CH7 = 55,
	/* RX channels 56 thru 60 are unused */
	STEDMA40_DEV_CAC0_RX = 61,
	/* RX channels 62 and 63 are unused */
};

enum dma_dest_dev_type {
	STEDMA40_DEV_SPI0_TX = 0,
	STEDMA40_DEV_SD_MMC0_TX = 1,
	STEDMA40_DEV_SD_MMC1_TX = 2,
	STEDMA40_DEV_SD_MMC2_TX = 3,
	STEDMA40_DEV_I2C1_TX = 4,
	STEDMA40_DEV_I2C3_TX = 5,
	STEDMA40_DEV_I2C2_TX = 6,
	STEDMA50_DEV_I2C4_TX = 7, /* Only on V1 */
	STEDMA40_DEV_SSP0_TX = 8,
	STEDMA40_DEV_SSP1_TX = 9,
	/* TX channel 10 is unused */
	STEDMA40_DEV_UART2_TX = 11,
	STEDMA40_DEV_UART1_TX = 12,
	STEDMA40_DEV_UART0_TX= 13,
	STEDMA40_DEV_MSP2_TX = 14,
	STEDMA40_DEV_I2C0_TX = 15,
	STEDMA40_DEV_USB_OTG_OEP_8 = 16,
	STEDMA40_DEV_USB_OTG_OEP_1_9 = 17,
	STEDMA40_DEV_USB_OTG_OEP_2_10= 18,
	STEDMA40_DEV_USB_OTG_OEP_3_11 = 19,
	STEDMA40_DEV_SLIM0_CH0_TX_HSI_TX_CH0 = 20,
	STEDMA40_DEV_SLIM0_CH1_TX_HSI_TX_CH1 = 21,
	STEDMA40_DEV_SLIM0_CH2_TX_HSI_TX_CH2 = 22,
	STEDMA40_DEV_SLIM0_CH3_TX_HSI_TX_CH3 = 23,
	STEDMA40_DEV_DST_SXA0_RX_TX = 24,
	STEDMA40_DEV_DST_SXA1_RX_TX = 25,
	STEDMA40_DEV_DST_SXA2_RX_TX = 26,
	STEDMA40_DEV_DST_SXA3_RX_TX = 27,
	STEDMA40_DEV_SD_MM2_TX = 28,
	STEDMA40_DEV_SD_MM0_TX = 29,
	STEDMA40_DEV_MSP1_TX = 30,
	/*
	 * This channel is either SlimBus or MSP,
	 * never both at the same time.
	 */
	STEDMA40_SLIM0_CH0_TX = 31,
	STEDMA40_DEV_MSP0_TX = 31,
	STEDMA40_DEV_SD_MM1_TX = 32,
	STEDMA40_DEV_SPI2_TX = 33,
	/* Secondary I2C3 channel */
	STEDMA40_DEV_I2C3_TX2 = 34,
	STEDMA40_DEV_SPI1_TX = 35,
	STEDMA40_DEV_USB_OTG_OEP_4_12 = 36,
	STEDMA40_DEV_USB_OTG_OEP_5_13 = 37,
	STEDMA40_DEV_USB_OTG_OEP_6_14 = 38,
	STEDMA40_DEV_USB_OTG_OEP_7_15 = 39,
	STEDMA40_DEV_SPI3_TX = 40,
	STEDMA40_DEV_SD_MM3_TX = 41,
	STEDMA40_DEV_SD_MM4_TX = 42,
	STEDMA40_DEV_SD_MM5_TX = 43,
	STEDMA40_DEV_DST_SXA4_RX_TX = 44,
	STEDMA40_DEV_DST_SXA5_RX_TX = 45,
	STEDMA40_DEV_DST_SXA6_RX_TX = 46,
	STEDMA40_DEV_DST_SXA7_RX_TX = 47,
	STEDMA40_DEV_CAC1_TX = 48,
	STEDMA40_DEV_CAC1_TX_HAC1_TX = 49,
	STEDMA40_DEV_HAC1_TX = 50,
	STEDMA40_MEMXCPY_TX_0 = 51,
	STEDMA40_DEV_SLIM1_CH0_TX_HSI_TX_CH4 = 52,
	STEDMA40_DEV_SLIM1_CH1_TX_HSI_TX_CH5 = 53,
	STEDMA40_DEV_SLIM1_CH2_TX_HSI_TX_CH6 = 54,
	STEDMA40_DEV_SLIM1_CH3_TX_HSI_TX_CH7 = 55,
	STEDMA40_MEMCPY_TX_1 = 56,
	STEDMA40_MEMCPY_TX_2 = 57,
	STEDMA40_MEMCPY_TX_3 = 58,
	STEDMA40_MEMCPY_TX_4 = 59,
	STEDMA40_MEMCPY_TX_5 = 60,
	STEDMA40_DEV_CAC0_TX = 61,
	STEDMA40_DEV_CAC0_TX_HAC0_TX = 62,
	STEDMA40_DEV_HAC0_TX = 63,
};

#endif
