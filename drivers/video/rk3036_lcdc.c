/*
 * (C) Copyright 2008-2014 Rockchip Electronics
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <common.h>
#include <lcd.h>
#include <asm/arch/rkplat.h>

DECLARE_GLOBAL_DATA_PTR;

#define LcdReadBit(addr, msk)      ((regbak.addr=preg->addr)&(msk))
#define LcdWrReg(addr, val)        preg->addr=regbak.addr=(val)
#define LcdRdReg(addr)             (preg->addr)
#define LcdSetBit(addr, msk)       preg->addr=((regbak.addr) |= (msk))
#define LcdClrBit(addr, msk)       preg->addr=((regbak.addr) &= ~(msk))
#define LcdSetRegBit(addr, msk)    preg->addr=((preg->addr) |= (msk))
#define LcdMskReg(addr, msk, val)  (regbak.addr)&=~(msk);   preg->addr=(regbak.addr|=(val))
#define LCDC_REG_CFG_DONE()		LcdWrReg(REG_CFG_DONE, 0x01); 


/********************************************************************
**                          �ṹ����                                *
********************************************************************/
/* LCDC�ļĴ����ṹ */
typedef volatile struct tagLCDC_REG
{
    /* offset 0x00~0xDC */
    unsigned int SYS_CTRL;		//(0x00)
    unsigned int DSP_CTRL0;		//(0x04)
    unsigned int DSP_CTRL1;		//(0x08)
    unsigned int FS_ADDR_MASK;		//(0x0C)
     
    unsigned int INT_STATUS;		//(0x10)
    unsigned int ALPHA_CTRL;		//(0x14)
    unsigned int WIN0_COLOR_KEY;	//(0x18)
    unsigned int WIN1_COLOR_KEY;	//(0x1C)
     
    unsigned int WIN0_YRGB_MST; 	//(0x20)
    unsigned int WIN0_CBR_MST;		//(0x24)
    unsigned int WIN1_VIR;		//(0x28)
    unsigned int AXI_BUS_CTRL;		//(0x2C)
     
    unsigned int WIN0_VIR;		//(0x30)
    unsigned int WIN0_ACT_INFO; 	//(0x34)
    unsigned int WIN0_DSP_INFO; 	//(0x38)
    unsigned int WIN0_DSP_ST;		//(0x3C)
     
    unsigned int WIN0_SCL_FACTOR_YRGB;	//(0x40)
    unsigned int WIN0_SCL_FACTOR_CBR;	//(0x44)
    unsigned int WIN0_SCL_OFFSET;	//(0x48)
    unsigned int RESERVED0[3];		//(0x4C)
     
    unsigned int HWC_MST;		//(0x58)
    unsigned int HWC_DSP_ST;		//(0x5C)
     
    unsigned int RESERVED1[3];		//(0x60)
    unsigned int DSP_HTOTAL_HS_END;	//(0x6C)
     
    unsigned int DSP_HACT_ST_END;	//(0x70)
    unsigned int DSP_VTOTAL_VS_END;	//(0x74)
    unsigned int DSP_VACT_ST_END;	//(0x78)
    unsigned int DSP_VS_ST_END_F1;	//(0x7C)
     
    unsigned int DSP_VACT_ST_END_F1;	//(0x80)
    unsigned int GATHER_TRANSFER;	//(0x84)
    unsigned int RESERVED2[2];		//(0x88)
     
    unsigned int REG_CFG_DONE;		//(0x90)
    unsigned int VERSION_INFO;		//(0x94)
    unsigned int RESERVED3[2];		//(0x98)
     
    unsigned int WIN1_MST;		//(0xa0)
    unsigned int RESERVED4[4];		//(0xa4)
     
    unsigned int WIN1_ACT_INFO; 	//(0xB4)
    unsigned int WIN1_DSP_INFO; 	//(0xB8)
    unsigned int WIN1_DSP_ST;		//(0xBC)
     
    unsigned int WIN1_SCL_FACTOR_YRGB;	//(0xC0)
    unsigned int RESERVED5;		//(0xc4)
    unsigned int WIN1_SCL_OFFSET;	//(0xC8)
    unsigned int RESERVED6;		//(0xCC)
     
    unsigned int BCSH_CTRL;		//(0xD0)
    unsigned int BCSH_COLOR_BAR;	//(0xD4)
    unsigned int BCSH_BCS;		//(0xD8)
    unsigned int BCSH_H;		//(0xDC)
} LCDC_REG, *pLCDC_REG;

/*******************register definition**********************/
//#define SYS_CTRL 		(0x00)
#define m_WIN0_EN		(1<<0)
#define m_WIN1_EN		(1<<1)
#define m_HWC_EN		(1<<2)
#define m_WIN0_FORMAT		(7<<3)
#define m_WIN1_FORMAT		(7<<6)
#define m_HWC_LUT_EN		(1<<9)
#define m_HWC_SIZE		(1<<10)
#define m_WIN0_RB_SWAP		(1<<15)
#define m_WIN0_ALPHA_SWAP	(1<<16)
#define m_WIN0_Y8_SWAP		(1<<17)
#define m_WIN0_UV_SWAP		(1<<18)
#define m_WIN1_RB_SWAP		(1<<19)
#define m_WIN1_ALPHA_SWAP	(1<<20)
#define m_WIN0_OTSD_DISABLE	(1<<22)
#define m_WIN1_OTSD_DISABLE	(1<<23)
#define m_DMA_BURST_LENGTH	(3<<24)
#define m_HWC_LODAD_EN		(1<<26)
#define m_DMA_STOP		(1<<29)
#define m_LCDC_STANDBY		(1<<30)
#define m_AUTO_GATING_EN	(1<<31)

#define v_WIN0_EN(x)		(((x)&1)<<0)
#define v_WIN1_EN(x)		(((x)&1)<<1)
#define v_HWC_EN(x)		(((x)&1)<<2)
#define v_WIN0_FORMAT(x)	(((x)&7)<<3)
#define v_WIN1_FORMAT(x)	(((x)&7)<<6)
#define v_HWC_LUT_EN(x)		(((x)&1)<<9)
#define v_HWC_SIZE(x)		(((x)&1)<<10)
#define v_WIN0_RB_SWAP(x)	(((x)&1)<<15)
#define v_WIN0_ALPHA_SWAP(x)	(((x)&1)<<16)
#define v_WIN0_Y8_SWAP(x)	(((x)&1)<<17)
#define v_WIN0_UV_SWAP(x)	(((x)&1)<<18)
#define v_WIN1_RB_SWAP(x)	(((x)&1)<<19)
#define v_WIN1_ALPHA_SWAP(x)	(((x)&1)<<20)
#define v_WIN0_OTSD_DISABLE(x)	(((x)&1)<<22)
#define v_WIN1_OTSD_DISABLE(x)	(((x)&1)<<23)
#define v_DMA_BURST_LENGTH(x)	(((x)&3)<<24)
#define v_HWC_LODAD_EN(x)	(((x)&1)<<26)
#define v_WIN1_LUT_EN(x)	(((x)&1)<<27)
#define v_DMA_STOP(x)		(((x)&1)<<29)
#define v_LCDC_STANDBY(x)	(((x)&1)<<30)
#define v_AUTO_GATING_EN(x)	(((x)&1)<<31)

//#define DSP_CTRL0		(0x04)
#define m_DSP_OUT_FORMAT	(0x0f<<0)
#define m_HSYNC_POL		(1<<4)
#define m_VSYNC_POL		(1<<5)
#define m_DEN_POL		(1<<6)
#define m_DCLK_POL		(1<<7)
#define m_WIN0_TOP		(1<<8)
#define m_DITHER_UP_EN		(1<<9)
#define m_DITHER_DOWN_MODE	(1 << 10)
#define m_DITHER_DOWN_EN	(1 << 11)
#define m_INTERLACE_DSP_EN	(1<<12)
#define m_WIN1_INTERLACE_EN	(1<<15)
#define m_WIN0_YRGB_DEFLICK_EN	(1<<16)
#define m_WIN0_CBR_DEFLICK_EN	(1<<17)
#define m_WIN0_ALPHA_MODE	(1<<18)
#define m_WIN1_ALPHA_MODE	(1<<19)
#define m_WIN0_CSC_MODE		(3<<20)
#define m_WIN0_YUV_CLIP		(1<<23)
#define m_TVE_MODE		(1<<25)
#define m_DITHER_DOWN_SEL	(1 << 27)
#define m_HWC_ALPHA_MODE	(1<<28)
#define m_PREMUL_ALPHA_ENABLE	(1<<29)
#define m_ALPHA_MODE_SEL1	(1<<30)
#define m_WIN1_DIFF_DCLK_EN	(1<<31)

#define v_DSP_OUT_FORMAT(x)	(((x)&0x0f)<<0)
#define v_HSYNC_POL(x)		(((x)&1)<<4)
#define v_VSYNC_POL(x)		(((x)&1)<<5)
#define v_DEN_POL(x)		(((x)&1)<<6)
#define v_DCLK_POL(x)		(((x)&1)<<7)
#define v_WIN0_TOP(x)		(((x)&1)<<8)
#define v_DITHER_UP_EN(x)	(((x)&1)<<9)
#define v_DITHER_DOWN_MODE(x)	((x&1) << 10)
#define v_DITHER_DOWN_EN(x)	((x&1) << 11)

#define v_INTERLACE_DSP_EN(x)	(((x)&1)<<12)
#define v_WIN1_INTERLACE_EN(x)	(((x)&1)<<15)
#define v_WIN0_YRGB_DEFLICK_EN(x)	(((x)&1)<<16)
#define v_WIN0_CBR_DEFLICK_EN(x)	(((x)&1)<<17)
#define v_WIN0_ALPHA_MODE(x)		(((x)&1)<<18)
#define v_WIN1_ALPHA_MODE(x)		(((x)&1)<<19)
#define v_WIN0_CSC_MODE(x)		(((x)&3)<<20)
#define v_WIN0_YUV_CLIP(x)		(((x)&1)<<23)
#define v_TVE_MODE(x)			(((x)&1)<<25)
#define v_DITHER_DOWN_SEL(x)		((x&1) << 27)
#define v_HWC_ALPHA_MODE(x)		(((x)&1)<<28)
#define v_PREMUL_ALPHA_ENABLE(x)	(((x)&1)<<29)
#define v_ALPHA_MODE_SEL1(x)		(((x)&1)<<30)
#define v_WIN1_DIFF_DCLK_EN(x)		(((x)&1)<<31)

//#define DSP_CTRL1		(0x08)
#define m_BG_COLOR		(0xffffff<<0)
#define m_BG_B			(0xff<<0)
#define m_BG_G			(0xff<<8)
#define m_BG_R			(0xff<<16)
#define m_BLANK_EN		(1<<24)
#define m_BLACK_EN		(1<<25)
#define m_DSP_BG_SWAP		(1<<26)
#define m_DSP_RB_SWAP		(1<<27)
#define m_DSP_RG_SWAP		(1<<28)
#define m_DSP_DELTA_SWAP	(1<<29)
#define m_DSP_DUMMY_SWAP	(1<<30)
#define m_DSP_OUT_ZERO		(1<<31)

#define v_BG_COLOR(x)		(((x)&0xffffff)<<0)
#define v_BG_B(x)		(((x)&0xff)<<0)
#define v_BG_G(x)		(((x)&0xff)<<8)
#define v_BG_R(x)		(((x)&0xff)<<16)
#define v_BLANK_EN(x)		(((x)&1)<<24)
#define v_BLACK_EN(x)		(((x)&1)<<25)
#define v_DSP_BG_SWAP(x)	(((x)&1)<<26)
#define v_DSP_RB_SWAP(x)	(((x)&1)<<27)
#define v_DSP_RG_SWAP(x)	(((x)&1)<<28)
#define v_DSP_DELTA_SWAP(x)	(((x)&1)<<29)
#define v_DSP_DUMMY_SWAP(x)	(((x)&1)<<30)
#define v_DSP_OUT_ZERO(x)	(((x)&1)<<31)

//#define INT_STATUS		(0x10)
#define m_HS_INT_STA		(1<<0)  //status
#define m_FS_INT_STA		(1<<1)
#define m_LF_INT_STA		(1<<2)
#define m_BUS_ERR_INT_STA	(1<<3)
#define m_HS_INT_EN		(1<<4)  //enable
#define m_FS_INT_EN          	(1<<5)
#define m_LF_INT_EN         	(1<<6)
#define m_BUS_ERR_INT_EN	(1<<7)
#define m_HS_INT_CLEAR		(1<<8) //auto clear
#define m_FS_INT_CLEAR		(1<<9)
#define m_LF_INT_CLEAR		(1<<10)
#define m_BUS_ERR_INT_CLEAR	(1<<11)
#define m_LF_INT_NUM		(0xfff<<12)
#define m_WIN0_EMPTY_INT_EN	(1<<24)
#define m_WIN1_EMPTY_INT_EN	(1<<25)
#define m_WIN0_EMPTY_INT_CLEAR	(1<<26)
#define m_WIN1_EMPTY_INT_CLEAR	(1<<27)
#define m_WIN0_EMPTY_INT_STA	(1<<28)
#define m_WIN1_EMPTY_INT_STA	(1<<29)
#define m_FS_RAW_STA		(1<<30)
#define m_LF_RAW_STA		(1<<31)

#define v_HS_INT_EN(x)			(((x)&1)<<4)
#define v_FS_INT_EN(x)			(((x)&1)<<5)
#define v_LF_INT_EN(x)			(((x)&1)<<6)
#define v_BUS_ERR_INT_EN(x)		(((x)&1)<<7)
#define v_HS_INT_CLEAR(x)		(((x)&1)<<8)
#define v_FS_INT_CLEAR(x)		(((x)&1)<<9)
#define v_LF_INT_CLEAR(x)		(((x)&1)<<10)
#define v_BUS_ERR_INT_CLEAR(x)		(((x)&1)<<11)
#define v_LF_INT_NUM(x)			(((x)&0xfff)<<12)
#define v_WIN0_EMPTY_INT_EN(x)		(((x)&1)<<24)
#define v_WIN1_EMPTY_INT_EN(x)		(((x)&1)<<25)
#define v_WIN0_EMPTY_INT_CLEAR(x)	(((x)&1)<<26)
#define v_WIN1_EMPTY_INT_CLEAR(x)	(((x)&1)<<27)

//#define ALPHA_CTRL		(0x14)
#define m_WIN0_ALPHA_EN		(1<<0)
#define m_WIN1_ALPHA_EN		(1<<1)
#define m_HWC_ALPAH_EN		(1<<2)
#define m_WIN1_PREMUL_SCALE	(1<<3)
#define m_WIN0_ALPHA_VAL	(0xff<<4)
#define m_WIN1_ALPHA_VAL	(0xff<<12)
#define m_HWC_ALPAH_VAL		(0x0f<<20)

#define v_WIN0_ALPHA_EN(x)	(((x)&1)<<0)
#define v_WIN1_ALPHA_EN(x)	(((x)&1)<<1)
#define v_HWC_ALPAH_EN(x)	(((x)&1)<<2)
#define v_WIN1_PREMUL_SCALE(x)	(((x)&1)<<3)
#define v_WIN0_ALPHA_VAL(x)	(((x)&0xff)<<4)
#define v_WIN1_ALPHA_VAL(x)	(((x)&0xff)<<12)
#define v_HWC_ALPAH_VAL(x)	(((x)&0x0f)<<20)

//#define WIN0_COLOR_KEY		(0x18)
//#define WIN1_COLOR_KEY		(0x1C)
#define m_COLOR_KEY_VAL		(0xffffff<<0)
#define m_COLOR_KEY_EN		(1<<24)
#define v_COLOR_KEY_VAL(x)	(((x)&0xffffff)<<0)
#define v_COLOR_KEY_EN(x)	(((x)&1)<<24)

/* Layer Registers */
//#define WIN0_YRGB_MST		(0x20)
//#define WIN0_CBR_MST		(0x24)
//#define WIN1_MST		(0xa0)
//#define HWC_MST			(0x58)

//#define WIN1_VIR		(0x28)
//#define WIN0_VIR		(0x30)
#define m_YRGB_VIR	(0x1fff << 0)
#define m_CBBR_VIR	(0x1fff << 16)

#define v_YRGB_VIR(x)	((x & 0x1fff) << 0)
#define v_CBBR_VIR(x)	((x & 0x1fff) << 16)

#define v_ARGB888_VIRWIDTH(x)	(((x)&0x1fff)<<0)
#define v_RGB888_VIRWIDTH(x) 	(((((x*3)>>2)+((x)%3))&0x1fff)<<0)
#define v_RGB565_VIRWIDTH(x)	((DIV_ROUND_UP(x,2)&0x1fff)<<0)
#define v_YUV_VIRWIDTH(x)	((DIV_ROUND_UP(x,4)&0x1fff)<<0)
#define v_CBCR_VIR(x)		((x & 0x1fff) << 16)

//#define WIN0_ACT_INFO		(0x34)
//#define WIN1_ACT_INFO		(0xB4)
#define m_ACT_WIDTH       	(0x1fff<<0)
#define m_ACT_HEIGHT      	(0x1fff<<16)
#define v_ACT_WIDTH(x)       	(((x-1)&0x1fff)<<0)
#define v_ACT_HEIGHT(x)      	(((x-1)&0x1fff)<<16)

//#define WIN0_DSP_INFO		(0x38)
//#define WIN1_DSP_INFO		(0xB8)
#define v_DSP_WIDTH(x)     	(((x-1)&0x7ff)<<0)
#define v_DSP_HEIGHT(x)    	(((x-1)&0x7ff)<<16)

//#define WIN0_DSP_ST		(0x3C)
//#define WIN1_DSP_ST		(0xBC)
//#define HWC_DSP_ST		(0x5C)
#define v_DSP_STX(x)      	(((x)&0xfff)<<0)
#define v_DSP_STY(x)      	(((x)&0xfff)<<16)

//#define WIN0_SCL_FACTOR_YRGB	(0x40)
//#define WIN0_SCL_FACTOR_CBR	(0x44)
//#define WIN1_SCL_FACTOR_YRGB	(0xC0)
#define v_X_SCL_FACTOR(x)  	(((x)&0xffff)<<0)
#define v_Y_SCL_FACTOR(x)  	(((x)&0xffff)<<16)

//#define WIN0_SCL_OFFSET		(0x48)
//#define WIN1_SCL_OFFSET		(0xC8)

/* LUT Registers */
//#define WIN1_LUT_ADDR 			(0x0400)
#define HWC_LUT_ADDR   			(0x0800)

/* Display Infomation Registers */
//#define DSP_HTOTAL_HS_END	(0x6C)
#define v_HSYNC(x)  		(((x)&0xfff)<<0)   //hsync pulse width
#define v_HORPRD(x) 		(((x)&0xfff)<<16)   //horizontal period

//#define DSP_HACT_ST_END		(0x70)
#define v_HAEP(x) 		(((x)&0xfff)<<0)  //horizontal active end point
#define v_HASP(x) 		(((x)&0xfff)<<16) //horizontal active start point

//#define DSP_VTOTAL_VS_END	(0x74)
#define v_VSYNC(x) 		(((x)&0xfff)<<0)
#define v_VERPRD(x) 		(((x)&0xfff)<<16)

//#define DSP_VACT_ST_END		(0x78)
#define v_VAEP(x) 		(((x)&0xfff)<<0)
#define v_VASP(x) 		(((x)&0xfff)<<16)

//#define DSP_VS_ST_END_F1	(0x7C)
#define v_VSYNC_END_F1(x) 		(((x)&0xfff)<<0)
#define v_VSYNC_ST_F1(x) 		(((x)&0xfff)<<16)
//#define DSP_VACT_ST_END_F1	(0x80)
/* BCSH Registers */
//#define BCSH_CTRL		(0xd0)
#define m_BCSH_EN		BITS(1, 0)
#define m_BCSH_R2Y_CSC_MODE     BITS(1, 1)       /* rk312x */
#define m_BCSH_OUT_MODE		BITS(3, 2)
#define m_BCSH_Y2R_CSC_MODE     BITS(3, 4)
#define m_BCSH_Y2R_EN           BITS(1, 6)       /* rk312x */
#define m_BCSH_R2Y_EN           BITS(1, 7)       /* rk312x */

#define v_BCSH_EN(x)		BITS_MASK(x, 1, 0)
#define v_BCSH_R2Y_CSC_MODE(x)  BITS_MASK(x, 1, 1)       /* rk312x */
#define v_BCSH_OUT_MODE(x)	BITS_MASK(x, 3, 2)
#define v_BCSH_Y2R_CSC_MODE(x)	BITS_MASK(x, 3, 4)
#define v_BCSH_Y2R_EN(x)        BITS_MASK(x, 1, 6)       /* rk312x */
#define v_BCSH_R2Y_EN(x)        BITS_MASK(x, 1, 7)       /* rk312x */

//#define BCSH_COLOR_BAR 		(0xd4)
#define m_BCSH_COLOR_BAR_Y      BITS(0xff, 0)
#define m_BCSH_COLOR_BAR_U	BITS(0xff, 8)
#define m_BCSH_COLOR_BAR_V	BITS(0xff, 16)

#define v_BCSH_COLOR_BAR_Y(x)	BITS_MASK(x, 0xff, 0)
#define v_BCSH_COLOR_BAR_U(x)   BITS_MASK(x, 0xff, 8)
#define v_BCSH_COLOR_BAR_V(x)   BITS_MASK(x, 0xff, 16)

//#define BCSH_BCS 		(0xd8)
#define m_BCSH_BRIGHTNESS	BITS(0x1f, 0)
#define m_BCSH_CONTRAST		BITS(0xff, 8)
#define m_BCSH_SAT_CON		BITS(0x1ff, 16)

#define v_BCSH_BRIGHTNESS(x)	BITS_MASK(x, 0x1f, 0)
#define v_BCSH_CONTRAST(x)	BITS_MASK(x, 0xff, 8)
#define v_BCSH_SAT_CON(x)       BITS_MASK(x, 0x1ff, 16)

//#define BCSH_H 			(0xdc)
#define m_BCSH_SIN_HUE		BITS(0xff, 0)
#define m_BCSH_COS_HUE		BITS(0xff, 8)

#define v_BCSH_SIN_HUE(x)	BITS_MASK(x, 0xff, 0)
#define v_BCSH_COS_HUE(x)	BITS_MASK(x, 0xff, 8)

/* Bus Register */
//#define AXI_BUS_CTRL		(0x2C)
#define m_IO_PAD_CLK			(1 << 31)
#define m_CORE_CLK_DIV_EN		(1 << 30)
#define m_MIPI_DCLK_INV			(1 << 29)
#define m_MIPI_DCLK_EN			(1 << 28)
#define m_LVDS_DCLK_INV			(1 << 27)
#define m_LVDS_DCLK_EN			(1 << 26)
#define m_RGB_DCLK_INV			(1 << 25)
#define m_RGB_DCLK_EN			(1 << 24)
#define m_HDMI_DCLK_INVERT		(1 << 23)
#define m_HDMI_DCLK_EN			(1 << 22)
#define m_TVE_DAC_DCLK_INVERT		(1 << 21)
#define m_TVE_DAC_DCLK_EN		(1 << 20)
#define m_HDMI_DCLK_DIV_EN		(1 << 19)
#define m_AXI_OUTSTANDING_MAX_NUM	(0x1f << 12)
#define m_AXI_MAX_OUTSTANDING_EN	(1 << 11)
#define m_MMU_EN			(1 << 10)
#define m_NOC_HURRY_THRESHOLD		(0xf << 6)
#define m_NOC_HURRY_VALUE		(3 << 4)
#define m_NOC_HURRY_EN			(1 << 3)
#define m_NOC_QOS_VALUE			(3 << 1)
#define m_NOC_QOS_EN			(1 << 0)

#define v_IO_PAD_CLK(x)			((x&1) << 31)
#define v_CORE_CLK_DIV_EN(x)		((x&1) << 30)
#define v_MIPI_DCLK_INV(x)		((x&1) << 29)
#define v_MIPI_DCLK_EN(x)		((x&1) << 28)
#define v_LVDS_DCLK_INV(x)		((x&1) << 27)
#define v_LVDS_DCLK_EN(x)		((x&1) << 26)
#define v_RGB_DCLK_INV(x)		((x&1) << 25)
#define v_RGB_DCLK_EN(x)		((x&1) << 24)

#define v_HDMI_DCLK_INVERT(x)		((x&1) << 23)
#define v_HDMI_DCLK_EN(x)		((x&1) << 22)
#define v_TVE_DAC_DCLK_INVERT(x)	((x&1) << 21)
#define v_TVE_DAC_DCLK_EN(x)		((x&1) << 20)
#define v_HDMI_DCLK_DIV_EN(x)		((x&1) << 19)
#define v_AXI_OUTSTANDING_MAX_NUM(x)	((x&0x1f) << 12)
#define v_AXI_MAX_OUTSTANDING_EN(x)	((x&1) << 11)
#define v_MMU_EN(x)			((x&1) << 10)
#define v_NOC_HURRY_THRESHOLD(x)	((x&0xf) << 6)
#define v_NOC_HURRY_VALUE(x)		((x&3) << 4)
#define v_NOC_HURRY_EN(x)		((x&1) << 3)
#define v_NOC_QOS_VALUE(x)		((x&3) << 1)
#define v_NOC_QOS_EN(x)			((x&1) << 0)

//#define GATHER_TRANSFER		(0x84)
#define m_WIN1_AXI_GATHER_NUM		(0xf << 12)
#define m_WIN0_CBCR_AXI_GATHER_NUM	(0x7 << 8)
#define m_WIN0_YRGB_AXI_GATHER_NUM	(0xf << 4)
#define m_WIN1_AXI_GAHTER_EN		(1 << 2)
#define m_WIN0_CBCR_AXI_GATHER_EN	(1 << 1)
#define m_WIN0_YRGB_AXI_GATHER_EN	(1 << 0)

#define v_WIN1_AXI_GATHER_NUM(x)	((x & 0xf) << 12)
#define v_WIN0_CBCR_AXI_GATHER_NUM(x)	((x & 0x7) << 8)
#define v_WIN0_YRGB_AXI_GATHER_NUM(x)	((x & 0xf) << 4)
#define v_WIN1_AXI_GAHTER_EN(x)		((x & 1) << 2)
#define v_WIN0_CBCR_AXI_GATHER_EN(x)	((x & 1) << 1)
#define v_WIN0_YRGB_AXI_GATHER_EN(x)	((x & 1) << 0)

//#define VERSION_INFO		(0x94)
#define m_MAJOR		(0xff << 24)
#define m_MINOR		(0xff << 16)
#define m_BUILD		(0xffff)
	
//#define REG_CFG_DONE		(0x90)
#define CalScale(x, y)	             ((((u32)(x-1))*0x1000)/(y-1))

enum _vop_tv_mode {
	TV_NTSC,
	TV_PAL,
};

LCDC_REG *preg = NULL;  
LCDC_REG regbak;

#define BITS(x, bit)            ((x) << (bit))
#define BITS_MASK(x, mask, bit)  BITS((x) & (mask), bit)
#define BITS_EN(mask, bit)       BITS(mask, bit + 16)

/* RK312X_GRF_LVDS_CON0 */
#define v_LVDS_DATA_SEL(x)      (BITS_MASK(x, 1, 0) | BITS_EN(1, 0))
#define v_LVDS_OUTPUT_FORMAT(x) (BITS_MASK(x, 3, 1) | BITS_EN(3, 1))
#define v_LVDS_MSBSEL(x)        (BITS_MASK(x, 1, 3) | BITS_EN(1, 3))
#define v_LVDSMODE_EN(x)        (BITS_MASK(x, 1, 6) | BITS_EN(1, 6))
#define v_MIPIPHY_TTL_EN(x)     (BITS_MASK(x, 1, 7) | BITS_EN(1, 7))
#define v_MIPIPHY_LANE0_EN(x)   (BITS_MASK(x, 1, 8) | BITS_EN(1, 8))
#define v_MIPIDPI_FORCEX_EN(x)  (BITS_MASK(x, 1, 9) | BITS_EN(1, 9))

#if (CONFIG_RKCHIPTYPE == CONFIG_RK3126) || (CONFIG_RKCHIPTYPE == CONFIG_RK3128)
enum {
        LVDS_DATA_FROM_LCDC = 0,
        LVDS_DATA_FORM_EBC,
};

enum {
        LVDS_MSB_D0 = 0,
        LVDS_MSB_D7,
};

enum _vop_r2y_csc_mode {
	VOP_R2Y_CSC_BT601 = 0,
	VOP_R2Y_CSC_BT709
};

enum _vop_y2r_csc_mode {
	VOP_Y2R_CSC_MPEG = 0,
	VOP_Y2R_CSC_JPEG,
	VOP_Y2R_CSC_HD,
	VOP_Y2R_CSC_BYPASS
};


/* RK312X_GRF_SOC_CON1 */
#define v_MIPITTL_CLK_EN(x)     (BITS_MASK(x, 1, 7) | BITS_EN(1, 7))
#define v_MIPITTL_LANE0_EN(x)   (BITS_MASK(x, 1, 11) | BITS_EN(1, 11))
#define v_MIPITTL_LANE1_EN(x)   (BITS_MASK(x, 1, 12) | BITS_EN(1, 12))
#define v_MIPITTL_LANE2_EN(x)   (BITS_MASK(x, 1, 13) | BITS_EN(1, 13))
#define v_MIPITTL_LANE3_EN(x)   (BITS_MASK(x, 1, 14) | BITS_EN(1, 14))


#define MIPIPHY_REG0            0x0000
#define m_LANE_EN_0             BITS(1, 2)
#define m_LANE_EN_1             BITS(1, 3)
#define m_LANE_EN_2             BITS(1, 4)
#define m_LANE_EN_3             BITS(1, 5)
#define m_LANE_EN_CLK           BITS(1, 5)
#define v_LANE_EN_0(x)          BITS(1, 2)
#define v_LANE_EN_1(x)          BITS(1, 3)
#define v_LANE_EN_2(x)          BITS(1, 4)
#define v_LANE_EN_3(x)          BITS(1, 5)
#define v_LANE_EN_CLK(x)        BITS(1, 5)

#define MIPIPHY_REG1            0x0004
#define m_SYNC_RST              BITS(1, 0)
#define m_LDO_PWR_DOWN          BITS(1, 1)
#define m_PLL_PWR_DOWN          BITS(1, 2)
#define v_SYNC_RST(x)           BITS_MASK(x, 1, 0)
#define v_LDO_PWR_DOWN(x)       BITS_MASK(x, 1, 1)
#define v_PLL_PWR_DOWN(x)       BITS_MASK(x, 1, 2)

#define MIPIPHY_REG3		0x000c
#define m_PREDIV                BITS(0x1f, 0)
#define m_FBDIV_MSB             BITS(1, 5)
#define v_PREDIV(x)             BITS_MASK(x, 0x1f, 0)
#define v_FBDIV_MSB(x)          BITS_MASK(x, 1, 5)

#define MIPIPHY_REG4		0x0010
#define v_FBDIV_LSB(x)          BITS_MASK(x, 0xff, 0)

#define MIPIPHY_REGE0		0x0380
#define m_MSB_SEL               BITS(1, 0)
#define m_DIG_INTER_RST         BITS(1, 2)
#define m_LVDS_MODE_EN          BITS(1, 5)
#define m_TTL_MODE_EN           BITS(1, 6)
#define m_MIPI_MODE_EN          BITS(1, 7)
#define v_MSB_SEL(x)            BITS_MASK(x, 1, 0)
#define v_DIG_INTER_RST(x)      BITS_MASK(x, 1, 2)
#define v_LVDS_MODE_EN(x)       BITS_MASK(x, 1, 5)
#define v_TTL_MODE_EN(x)        BITS_MASK(x, 1, 6)
#define v_MIPI_MODE_EN(x)       BITS_MASK(x, 1, 7)

#define MIPIPHY_REGE1           0x0384
#define m_DIG_INTER_EN          BITS(1, 7)
#define v_DIG_INTER_EN(x)       BITS_MASK(x, 1, 7)

#define MIPIPHY_REGE3           0x038c
#define m_MIPI_EN               BITS(1, 0)
#define m_LVDS_EN               BITS(1, 1)
#define m_TTL_EN                BITS(1, 2)
#define v_MIPI_EN(x)            BITS_MASK(x, 1, 0)
#define v_LVDS_EN(x)            BITS_MASK(x, 1, 1)
#define v_TTL_EN(x)             BITS_MASK(x, 1, 2)

#define MIPIPHY_REGE8           0x03a0

#define MIPIPHY_REGEB           0x03ac
#define v_PLL_PWR_OFF(x)        BITS_MASK(x, 1, 2)
#define v_LANECLK_EN(x)         BITS_MASK(x, 1, 3)
#define v_LANE3_EN(x)           BITS_MASK(x, 1, 4)
#define v_LANE2_EN(x)           BITS_MASK(x, 1, 5)
#define v_LANE1_EN(x)           BITS_MASK(x, 1, 6)
#define v_LANE0_EN(x)           BITS_MASK(x, 1, 7)



struct rk_lvds_device {
	int    base;
	bool   sys_state;
};

struct rk_lvds_device rk31xx_lvds;
static inline int lvds_writel(struct rk_lvds_device *lvds, u32 offset, u32 val)
{
	writel(val, lvds->base + offset);
	return 0;
}

static inline int lvds_msk_reg(struct rk_lvds_device *lvds, u32 offset,
			       u32 msk, u32 val)
{
	u32 temp;

	temp = readl(lvds->base + offset) & (0xFF - (msk));
	writel(temp | ((val) & (msk)), lvds->base + offset);
	return 0;
}

static inline u32 lvds_readl(struct rk_lvds_device *lvds, u32 offset)
{
	return readl(lvds->base + offset);
}




static int rk31xx_lvds_pwr_on(vidinfo_t * vid)
{
        struct rk_lvds_device *lvds = &rk31xx_lvds;

        if (vid->screen_type == SCREEN_LVDS) {
                /* power up lvds pll and ldo */
	        lvds_msk_reg(lvds, MIPIPHY_REG1,
	                     m_SYNC_RST | m_LDO_PWR_DOWN | m_PLL_PWR_DOWN,
	                     v_SYNC_RST(0) | v_LDO_PWR_DOWN(0) | v_PLL_PWR_DOWN(0));

	        /* enable lvds */
	        lvds_msk_reg(lvds, MIPIPHY_REGE3,
	                     m_MIPI_EN | m_LVDS_EN | m_TTL_EN,
	                     v_MIPI_EN(0) | v_LVDS_EN(1) | v_TTL_EN(0));
        } else {
                lvds_msk_reg(lvds, MIPIPHY_REGE3,
	                     m_MIPI_EN | m_LVDS_EN | m_TTL_EN,
	                     v_MIPI_EN(0) | v_LVDS_EN(0) | v_TTL_EN(1));
        }
        return 0;
}

static int rk31xx_lvds_pwr_off(void)
{
        struct rk_lvds_device *lvds = &rk31xx_lvds;

	/* power down lvds pll and bandgap */
	lvds_msk_reg(lvds, MIPIPHY_REG1,
	             m_SYNC_RST | m_LDO_PWR_DOWN | m_PLL_PWR_DOWN,
	             v_SYNC_RST(1) | v_LDO_PWR_DOWN(1) | v_PLL_PWR_DOWN(1));

	/* disable lvds */
	lvds_msk_reg(lvds, MIPIPHY_REGE3, m_LVDS_EN | m_TTL_EN,
	             v_LVDS_EN(0) | v_TTL_EN(0));
        return 0;
}

static void rk31xx_output_lvds(vidinfo_t *vid)
{
	u32 val = 0;
	struct rk_lvds_device *lvds = &rk31xx_lvds;

	/* if LVDS transmitter source from VOP, vop_dclk need get invert
	 * set iomux in dts pinctrl
	 */
	val = 0;
	val |= v_LVDSMODE_EN(1) | v_MIPIPHY_TTL_EN(0);  /* enable lvds mode */
	val |= v_LVDS_DATA_SEL(LVDS_DATA_FROM_LCDC);    /* config data source */
	val |= v_LVDS_OUTPUT_FORMAT(vid->lvds_format); /* config lvds_format */
	val |= v_LVDS_MSBSEL(LVDS_MSB_D7);      /* LSB receive mode */
	val |= v_MIPIPHY_LANE0_EN(1) | v_MIPIDPI_FORCEX_EN(1);
	grf_writel(val, GRF_LVDS_CON0);

	/* enable lvds lane */
	val = v_LANE0_EN(1) | v_LANE1_EN(1) | v_LANE2_EN(1) | v_LANE3_EN(1) |
	        v_LANECLK_EN(1) | v_PLL_PWR_OFF(0);
	lvds_writel(lvds, MIPIPHY_REGEB, val);

	/* set pll prediv and fbdiv */
	lvds_writel(lvds, MIPIPHY_REG3, v_PREDIV(2) | v_FBDIV_MSB(0));
	lvds_writel(lvds, MIPIPHY_REG4, v_FBDIV_LSB(28));

	lvds_writel(lvds, MIPIPHY_REGE8, 0xfc);

	/* set lvds mode and reset phy config */
	lvds_msk_reg(lvds, MIPIPHY_REGE0,
	             m_MSB_SEL | m_DIG_INTER_RST,
	             v_MSB_SEL(1) | v_DIG_INTER_RST(1));

	lvds_msk_reg(lvds, MIPIPHY_REGE1, m_DIG_INTER_EN, v_DIG_INTER_EN(1));

	rk31xx_lvds_pwr_on(vid);

}

static void rk31xx_output_lvttl(vidinfo_t *vid)
{
	u32 val = 0;
	struct rk_lvds_device *lvds = &rk31xx_lvds;

	grf_writel(0xfff35555, GRF_GPIO2B_IOMUX);
	grf_writel(0x00ff0055, GRF_GPIO2C_IOMUX);
	grf_writel(0x77771111, GRF_GPIO2C_IOMUX2);
	grf_writel(0x700c1004, GRF_GPIO2D_IOMUX);

	val |= v_LVDSMODE_EN(0) | v_MIPIPHY_TTL_EN(1);  /* enable lvds mode */
	val |= v_LVDS_DATA_SEL(LVDS_DATA_FROM_LCDC);    /* config data source */
	grf_writel(0xffff0380, GRF_LVDS_CON0);

	val = v_MIPITTL_CLK_EN(1) | v_MIPITTL_LANE0_EN(1) |
	        v_MIPITTL_LANE1_EN(1) | v_MIPITTL_LANE2_EN(1) |
	        v_MIPITTL_LANE3_EN(1);
	grf_writel(val, GRF_SOC_CON1);

	/* enable lane */
	lvds_writel(lvds, MIPIPHY_REG0, 0x7f);
	val = v_LANE0_EN(1) | v_LANE1_EN(1) | v_LANE2_EN(1) | v_LANE3_EN(1) |
	        v_LANECLK_EN(1) | v_PLL_PWR_OFF(1);
	lvds_writel(lvds, MIPIPHY_REGEB, val);

	/* set ttl mode and reset phy config */
	val = v_LVDS_MODE_EN(0) | v_TTL_MODE_EN(1) | v_MIPI_MODE_EN(0) |
	        v_MSB_SEL(1) | v_DIG_INTER_RST(1);
	lvds_writel(lvds, MIPIPHY_REGE0, val);

	rk31xx_lvds_pwr_on(vid);
		
}

static int rk31xx_lvds_en(vidinfo_t *vid)
{
	rk31xx_lvds.base = 0x20038000;

	switch (vid->screen_type) {
	case SCREEN_LVDS:
		rk31xx_output_lvds(vid);
	        break;
	case SCREEN_RGB:
		rk31xx_output_lvttl(vid);
	        break;
	default:
	        printf("unsupport screen type\n");
	        break;
	}
	return 0;
}

#endif

/* Configure VENC for a given Mode (NTSC / PAL) */
void rk30_lcdc_set_par(struct fb_dsp_info *fb_info, vidinfo_t *vid)
{
	u32 ScaleYrgbX=0;
	u32 ScaleYrgbY=0;
	
	struct layer_par *par = &vid->par[fb_info->layer_id];
	if(par == NULL){
		printf("%s lay_par==NULL,id=%d\n",fb_info->layer_id);
	}
	if(fb_info != &par->fb_info)
		memcpy(&par->fb_info,fb_info,sizeof(struct fb_dsp_info *));

	if (vid->vmode) {
		fb_info->ysize /= 2;
		fb_info->ypos  /= 2;
	}
	ScaleYrgbX = CalScale(fb_info->xact, fb_info->xsize);
	ScaleYrgbY = CalScale(fb_info->yact, fb_info->ysize);
	switch(fb_info->layer_id){
	case WIN0:
		LcdWrReg(WIN0_YRGB_MST, fb_info->yaddr);
		LcdWrReg(WIN0_SCL_FACTOR_YRGB,v_X_SCL_FACTOR(ScaleYrgbX) |
			v_Y_SCL_FACTOR(ScaleYrgbY));
		LcdMskReg(SYS_CTRL, m_WIN0_EN |m_WIN0_FORMAT,
				v_WIN0_EN(1)|v_WIN0_FORMAT(vid->logo_rgb_mode));
		LcdWrReg(WIN0_ACT_INFO,v_ACT_WIDTH(fb_info->xact) |
					v_ACT_HEIGHT(fb_info->yact));
		LcdWrReg(WIN0_DSP_ST, v_DSP_STX(fb_info->xpos + vid->vl_hspw + vid->vl_hbpd) |
				v_DSP_STY(fb_info->ypos + vid->vl_vspw + vid->vl_vbpd)); 
		LcdWrReg(WIN0_DSP_INFO, v_DSP_WIDTH(fb_info->xsize)|
				v_DSP_HEIGHT(fb_info->ysize));
		LcdMskReg(WIN0_COLOR_KEY, m_COLOR_KEY_EN | m_COLOR_KEY_VAL,
					v_COLOR_KEY_EN(0) | v_COLOR_KEY_VAL(0));

		switch(vid->logo_rgb_mode) 
		{
			case ARGB888:
				LcdWrReg(WIN0_VIR,v_ARGB888_VIRWIDTH(fb_info->xvir));  //zyw
				break;
			case RGB888:  //rgb888
				LcdWrReg(WIN0_VIR,v_RGB888_VIRWIDTH(fb_info->xvir));
				break;
			case RGB565:  //rgb565
				LcdWrReg(WIN0_VIR,v_RGB565_VIRWIDTH(fb_info->xvir));
				break;
			case YUV422:
			case YUV420:   
				LcdWrReg(WIN0_VIR,v_YUV_VIRWIDTH(fb_info->xvir));         
				break;
			default:
				LcdWrReg(WIN0_VIR,v_RGB888_VIRWIDTH(fb_info->xvir));
				break;
		}
		break;
	case WIN1:
		LcdWrReg(WIN1_SCL_FACTOR_YRGB,v_X_SCL_FACTOR(ScaleYrgbX) | 
						v_Y_SCL_FACTOR(ScaleYrgbY));
		LcdMskReg(SYS_CTRL, m_WIN1_EN |m_WIN1_FORMAT, 
			v_WIN1_EN(1)|v_WIN1_FORMAT(vid->logo_rgb_mode));

		LcdWrReg(WIN1_ACT_INFO,v_ACT_WIDTH(fb_info->xact) | v_ACT_HEIGHT(fb_info->yact));
		LcdWrReg(WIN1_DSP_ST, v_DSP_STX(fb_info->xpos + vid->vl_hspw + vid->vl_hbpd) | 
			v_DSP_STY(fb_info->ypos + vid->vl_vspw + vid->vl_vbpd)); 
		LcdWrReg(WIN1_DSP_INFO, v_DSP_WIDTH(fb_info->xsize)| v_DSP_HEIGHT(fb_info->ysize));
		LcdMskReg(WIN1_COLOR_KEY, m_COLOR_KEY_EN | m_COLOR_KEY_VAL,
				v_COLOR_KEY_EN(0) | v_COLOR_KEY_VAL(0));

		switch(vid->logo_rgb_mode) 
		{
			case ARGB888:
				LcdWrReg(WIN1_VIR,v_ARGB888_VIRWIDTH(fb_info->xvir));  //zyw
				break;
			case RGB888:  //rgb888
				LcdWrReg(WIN1_VIR,v_RGB888_VIRWIDTH(fb_info->xvir));
				break;
			case RGB565:  //rgb565
				LcdWrReg(WIN1_VIR,v_RGB565_VIRWIDTH(fb_info->xvir));
				break;
			case YUV422:
			case YUV420:   
				LcdWrReg(WIN1_VIR,v_YUV_VIRWIDTH(fb_info->xvir));         
				break;
			default:
				LcdWrReg(WIN1_VIR,v_RGB888_VIRWIDTH(fb_info->xvir));
				break;
		}
		LcdWrReg(WIN1_MST, fb_info->yaddr);
		break;
	default:
		printf("%s --->unknow lay_id \n");
		break;
	}

#if (CONFIG_RKCHIPTYPE == CONFIG_RK3128)
	if ((vid->screen_type == SCREEN_HDMI) ||
	   (vid->screen_type == SCREEN_TVOUT)) {
		LcdWrReg(BCSH_COLOR_BAR,0x00);
		LcdWrReg(BCSH_BCS,0x00808000);
		LcdWrReg(BCSH_H,0x00008000);
		LcdMskReg(BCSH_CTRL,
		m_BCSH_R2Y_EN | m_BCSH_R2Y_CSC_MODE | m_BCSH_Y2R_EN | m_BCSH_EN | m_BCSH_OUT_MODE,
		v_BCSH_R2Y_EN(1) | v_BCSH_R2Y_CSC_MODE(VOP_Y2R_CSC_MPEG) |
		v_BCSH_Y2R_EN(0) | v_BCSH_EN(1) | v_BCSH_OUT_MODE(3));
		LcdMskReg(DSP_CTRL1, m_BG_COLOR,v_BG_COLOR(0x800a80));
	}
#endif
	LCDC_REG_CFG_DONE();
	/*setenv("bootdelay", "3");*/
}

int rk30_load_screen(vidinfo_t *vid)
{
	int ret = -1;
	int face = 0;
	int mask,val;

	if (vid->screen_type == SCREEN_HDMI) {
        	LcdMskReg(AXI_BUS_CTRL,m_HDMI_DCLK_EN,v_HDMI_DCLK_EN(1));
		if (vid->pixelrepeat) {
			LcdMskReg(AXI_BUS_CTRL,m_CORE_CLK_DIV_EN,v_CORE_CLK_DIV_EN(1));
		}
    	} else if (vid->screen_type == SCREEN_TVOUT) {
		LcdMskReg(AXI_BUS_CTRL,m_TVE_DAC_DCLK_EN,v_TVE_DAC_DCLK_EN(1));
		if (vid->pixelrepeat) {
			LcdMskReg(AXI_BUS_CTRL,m_CORE_CLK_DIV_EN,v_CORE_CLK_DIV_EN(1));
		}
		if(vid->vl_col == 720 && vid->vl_row== 576) {
			LcdMskReg(DSP_CTRL0,m_TVE_MODE,v_TVE_MODE(TV_PAL));
		} else if(vid->vl_col == 720 && vid->vl_row== 480) {
			LcdMskReg(DSP_CTRL0,m_TVE_MODE,v_TVE_MODE(TV_NTSC));
		} else {
			printf("unsupported video timing!\n");
			return -1;
		}		
	} else if (vid->screen_type == SCREEN_LVDS) {
		LcdMskReg(AXI_BUS_CTRL, m_LVDS_DCLK_EN, v_LVDS_DCLK_EN(1));
	} else if(vid->screen_type == SCREEN_RGB) {
		LcdMskReg(AXI_BUS_CTRL, m_RGB_DCLK_EN | m_LVDS_DCLK_EN,
			v_RGB_DCLK_EN(1) | v_LVDS_DCLK_EN(1));
	} else if(vid->screen_type == SCREEN_MIPI){
		LcdMskReg(AXI_BUS_CTRL, m_MIPI_DCLK_EN, v_MIPI_DCLK_EN(1));
	}
 	
    	LcdMskReg(DSP_CTRL1, m_BLACK_EN|m_BLANK_EN|m_DSP_OUT_ZERO|m_DSP_BG_SWAP|m_DSP_RG_SWAP|
		m_DSP_RB_SWAP|m_BG_COLOR|m_DSP_DELTA_SWAP|m_DSP_DUMMY_SWAP, 
                         v_BLACK_EN(0)|v_BLANK_EN(0)|v_DSP_OUT_ZERO(0)|v_DSP_BG_SWAP(0)
                         |v_DSP_RG_SWAP(0)|v_DSP_RB_SWAP(0)|v_BG_COLOR(0)|v_DSP_DELTA_SWAP(0)|
                         v_DSP_DUMMY_SWAP(0));


	switch (vid->lcd_face)
	{
    	case OUT_P565:
        		face = OUT_P565;
        		//LcdMskReg(DSP_CTRL1, m_dither_down_en | m_dither_down_mode, v_dither_down_en(1) | v_dither_down_mode(0));
        		break;
    	case OUT_P666:
        		face = OUT_P666;
		        mask = m_DITHER_DOWN_EN |
                                m_DITHER_DOWN_MODE |
		                m_DITHER_DOWN_SEL;
		        val = v_DITHER_DOWN_EN(1) |
                                v_DITHER_DOWN_MODE(1) |
		                v_DITHER_DOWN_SEL(1);
        		//LcdMskReg(DSP_CTRL1, m_dither_down_en | m_dither_down_mode|m_dither_down_sel, v_dither_down_en(1) | v_dither_down_mode(1))|v_dither_down_sel(1);
        		break;
    	case OUT_D888_P565:
        		face = OUT_P888;
        		//LcdMskReg(DSP_CTRL1, m_dither_down_en | m_dither_down_mode, v_dither_down_en(1) | v_dither_down_mode(0));
        		break;
    	case OUT_D888_P666:
        		face = OUT_P888;
        		//LcdMskReg(DSP_CTRL1, m_dither_down_en | m_dither_down_mode|m_dither_down_sel, v_dither_down_en(1) | v_dither_down_mode(1)|v_dither_down_sel(1));
        		break; 
    	case OUT_P888:
        		face = OUT_P888;
        		//LcdMskReg(DSP_CTRL1, m_dither_down_en | m_dither_down_mode | m_dither_up_en, v_dither_down_en(0) | v_dither_down_mode(0) | v_dither_up_en(1));
        		break;
    	default:
        		//LcdMskReg(DSP_CTRL1, m_dither_down_en | m_dither_down_mode|m_dither_up_en, v_dither_down_en(0) | v_dither_down_mode(0) | v_dither_up_en(0));
        		face = vid->lcd_face;
        		break;
	}

	mask |= m_HSYNC_POL | m_HSYNC_POL | m_DEN_POL | m_DCLK_POL | m_DSP_OUT_FORMAT;
	val |=  v_HSYNC_POL(vid->vl_hsp) | v_VSYNC_POL(vid->vl_vsp) | v_DEN_POL(vid->vl_oep)|
		v_DCLK_POL(vid->vl_clkp) | v_DSP_OUT_FORMAT(face);
    	LcdMskReg(DSP_CTRL0, mask, val);

	val = v_HSYNC(vid->vl_hspw) | v_HORPRD(vid->vl_hspw + vid->vl_hbpd + vid->vl_col + vid->vl_hfpd);
	LcdWrReg(DSP_HTOTAL_HS_END,val);

	val = v_HAEP(vid->vl_hspw + vid->vl_hbpd + vid->vl_col) | v_HASP(vid->vl_hspw + vid->vl_hbpd);
	LcdWrReg(DSP_HACT_ST_END,val);

	if(vid->vmode) {
		//First Field Timing
		LcdWrReg(DSP_VTOTAL_VS_END, v_VSYNC(vid->vl_vspw) |
			    v_VERPRD(2 * (vid->vl_vspw + vid->vl_vbpd + vid->vl_vfpd) + vid->vl_row + 1));
		LcdWrReg(DSP_VACT_ST_END,v_VAEP(vid->vl_vspw + vid->vl_vbpd + vid->vl_row/2)|
			    v_VASP(vid->vl_vspw + vid->vl_vbpd));
		//Second Field Timing
		LcdWrReg(DSP_VS_ST_END_F1, v_VSYNC_ST_F1(vid->vl_vspw + vid->vl_vbpd + vid->vl_row/2 + vid->vl_vfpd) |
			    v_VSYNC_END_F1(2 * vid->vl_vspw + vid->vl_vbpd + vid->vl_row/2 + vid->vl_vfpd));
		LcdWrReg(DSP_VACT_ST_END_F1,v_VAEP(2 * (vid->vl_vspw + vid->vl_vbpd) + vid->vl_row + vid->vl_vfpd + 1)|
			    v_VASP(2 * (vid->vl_vspw + vid->vl_vbpd) + vid->vl_row/2 + vid->vl_vfpd + 1));
			    
		LcdMskReg(DSP_CTRL0, m_INTERLACE_DSP_EN | m_WIN1_INTERLACE_EN | m_WIN0_YRGB_DEFLICK_EN | m_WIN0_CBR_DEFLICK_EN, 
			v_INTERLACE_DSP_EN(1) | v_WIN1_INTERLACE_EN(0) | v_WIN0_YRGB_DEFLICK_EN(1) | v_WIN0_CBR_DEFLICK_EN(1) );

	} else {
		val = v_VSYNC(vid->vl_vspw) | 
		      v_VERPRD(vid->vl_vspw + vid->vl_vbpd + vid->vl_row + vid->vl_vfpd);
		LcdWrReg(DSP_VTOTAL_VS_END, val);
		val = v_VAEP(vid->vl_vspw + vid->vl_vbpd + vid->vl_row) |
		    	v_VASP(vid->vl_vspw + vid->vl_vbpd);
		LcdWrReg(DSP_VACT_ST_END,  val);
		LcdMskReg(DSP_CTRL0, m_INTERLACE_DSP_EN | m_WIN1_INTERLACE_EN | m_WIN0_YRGB_DEFLICK_EN | m_WIN0_CBR_DEFLICK_EN, 
			v_INTERLACE_DSP_EN(0) | v_WIN1_INTERLACE_EN(0) | v_WIN0_YRGB_DEFLICK_EN(0) | v_WIN0_CBR_DEFLICK_EN(0));
	}
	
	LCDC_REG_CFG_DONE();
#if (CONFIG_RKCHIPTYPE == CONFIG_RK3126) || (CONFIG_RKCHIPTYPE == CONFIG_RK3128)
	if ((vid->screen_type == SCREEN_LVDS) ||
	    (vid->screen_type == SCREEN_RGB)) {
		rk31xx_lvds_en(vid);
	} else if ((vid->screen_type == SCREEN_MIPI)||
		   (vid->screen_type == SCREEN_DUAL_MIPI)) {
		rk32_mipi_enable(vid);
	}
#endif
	return 0;
}

/* Enable LCD and DIGITAL OUT in DSS */
void rk30_lcdc_standby(enable)
{
	LcdMskReg(SYS_CTRL, m_LCDC_STANDBY, v_LCDC_STANDBY(enable ? 1 : 0));
	LCDC_REG_CFG_DONE();  
}

int rk_lcdc_init(int lcdc_id)
{
	if (gd->arch.chiptype == CONFIG_RK3036)
		preg = 0x10118000; 
	else
		preg = 0x1010e000;
	//grf_writel(1<<16, GRF_IO_VSEL); //LCDCIOdomain 3.3 Vvoltageselectio
	//lcdc_clk_enable();
	LcdMskReg(SYS_CTRL, m_AUTO_GATING_EN | m_LCDC_STANDBY | m_DMA_STOP, 
	                v_AUTO_GATING_EN(0)|v_LCDC_STANDBY(0)|v_DMA_STOP(0));
	
	LcdMskReg(AXI_BUS_CTRL, m_MMU_EN, v_MMU_EN(0));

	//LcdMskReg(INT_STATUS, m_FS_INT_EN, v_FS_INT_EN(1));  
	LCDC_REG_CFG_DONE();
	return 0;
}
