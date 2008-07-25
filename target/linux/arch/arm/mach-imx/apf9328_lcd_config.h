/*
 *
 * Armadeus LCD configuration file
 *
 * Put here all that is needed to configure the Hardware
 * interface to your LCD
 *
 *
 */

#ifndef APF9328_LCD_CONFIG_H
#define APF9328_LCD_CONFIG_H


#ifdef CONFIG_FB_IMX
#include <asm/arch/imxfb.h>
#include <linux/delay.h>

#define LCD_PORT                    3           /* Port D */

#define OPTREX_F51900_POWER_DOWN    7           /* PD7_REV line */
#define SHARP_LQ043_POWER_DOWN      12          /* PD12_ACD_OE line */
#define CONTRAST_LINE               11          /* PD11_CONTRAST */

#define DEFAULT_DMA_SETTINGS        (DMACR_BURST | DMACR_HM(8) | DMACR_TM(4))

/*
 * Power on/off LCD's internal logic
 */
static void apf9328_lcd_power(int on)
{
    pr_debug("apf9328_lcd_power: %s\n", on ? "on":"off");
#ifdef CONFIG_FB_IMX_OPTREX_F51900_CSTN
    if(on)
        DR(LCD_PORT) |= (1 << OPTREX_F51900_POWER_DOWN);
    else
        DR(LCD_PORT) &= ~(1 << OPTREX_F51900_POWER_DOWN);
//#elif CONFIG_FB_IMX_MICROTIPS_MTF_T035_TFT
    // No LCD powerdown yet
#elif CONFIG_FB_IMX_SHARP_LQ043_TFT
    if(on) {
        mdelay(200); // at least ten frames have to be processed before
                     // enabling the display
        DR(LCD_PORT) |= (1 << SHARP_LQ043_POWER_DOWN);
   }
    else
        DR(LCD_PORT) &= ~(1 << SHARP_LQ043_POWER_DOWN);
#else
    if(on)
        DR(LCD_PORT) &= ~(1 << CONTRAST_LINE);
    else
        DR(LCD_PORT) |= (1 << CONTRAST_LINE);
#endif // CONFIG_FB_IMX_OPTREX_F51900_CSTN
}

/*
 * Power on/off LCD's backlight
 */
static void apf9328_lcd_backlight_power(int on)
{
    pr_debug("apf9328_lcd_backlight_power: %s\n", on ? "on":"off");

    if(on)
        LCDC_PWMR |= PWMR_CC_EN;
    else
        LCDC_PWMR &= ~PWMR_CC_EN;
}


#ifdef CONFIG_FB_IMX_SHARP_LQ043_TFT
static struct imxfb_mach_info apf9328_fb_info __initdata = {
    .pixclock           = 62500,
    .bpp                = 16,
    .xres               = 480,
    .yres               = 272,

    .hsync_len          = 10,
    .vsync_len          = 3,
    .left_margin        = 2,
    .upper_margin       = 2,
    .right_margin       = 0,
    .lower_margin       = 3,

    .pcr                = PCR_TFT | PCR_COLOR | PCR_PBSIZ_8 | PCR_BPIX_16 | PCR_FLMPOL | PCR_LPPOL | 
                          PCR_CLKPOL | PCR_SCLKIDLE | PCR_SCLK_SEL | PCR_PCD(5),
    .pwmr               = 0x000001ff,    // Contrast with PWM @ Line_Pulse, max by default
    .dmacr              = DEFAULT_DMA_SETTINGS,
    .backlight_power    = apf9328_lcd_backlight_power,
    .lcd_power          = apf9328_lcd_power,
};
#elif CONFIG_FB_IMX_MICROTIPS_MTF_T035_TFT
static struct imxfb_mach_info apf9328_fb_info __initdata = {
    .pixclock           = 62500,
    .bpp                = 16,
    .xres               = 320,
    .yres               = 240,

    .hsync_len          = 30,
    .vsync_len          = 3,
    .left_margin        = 1,
    .upper_margin       = 20,
    .right_margin       = 68,
    .lower_margin       = 20,

    .pcr                = PCR_TFT | PCR_COLOR | PCR_PBSIZ_8 | PCR_BPIX_16 | PCR_FLMPOL | PCR_LPPOL |
                          PCR_SCLKIDLE | PCR_SCLK_SEL | PCR_PCD(7),
    .pwmr               = 0x000001ff,    // Contrast with PWM @ Line_Pulse, max by default
    .dmacr              = DEFAULT_DMA_SETTINGS,
    .backlight_power    = apf9328_lcd_backlight_power,
};
#elif CONFIG_FB_IMX_SHARP_LQ057_TFT
static struct imxfb_mach_info apf9328_fb_info __initdata = {
    .pixclock           = 62500,
    .bpp                = 16,
    .xres               = 320,
    .yres               = 240,

    .hsync_len          = 9,
    .vsync_len          = 2,
    .left_margin        = 9,
    .upper_margin       = 0,
    .right_margin       = 27,
    .lower_margin       = 7,

    .pcr                = PCR_TFT | PCR_COLOR | PCR_PBSIZ_8 | PCR_BPIX_16 | /*PCR_CLKPOL |*/
                          PCR_SCLKIDLE | PCR_SCLK_SEL | PCR_PCD(9),
    .pwmr               = 0,    // No contrast management
    .dmacr              = DEFAULT_DMA_SETTINGS,
    .lcd_power          = apf9328_lcd_power,
};
#elif CONFIG_FB_IMX_OPTREX_F51900_CSTN
static struct imxfb_mach_info apf9328_fb_info __initdata = {
    .pixclock           = 62500,
    .bpp                = 8,
    .xres               = 320,
    .yres               = 240,
 
    .hsync_len          = 2,
    .vsync_len          = 2,
    .left_margin        = 2,
    .upper_margin       = 2,
    .right_margin       = 2,
    .lower_margin       = 2,
 
    .pcr                = PCR_COLOR | PCR_PBSIZ_8 | PCR_BPIX_8 | PCR_ACD(5) | 
                          PCR_END_BYTE_SWAP | PCR_PCD(3),
    .dmacr              = DEFAULT_DMA_SETTINGS,
    .lcd_power          = apf9328_lcd_power,
};
#elif CONFIG_FB_IMX_MOTOROLA_A910_TFT
static struct imxfb_mach_info apf9328_fb_info __initdata = {
    .pixclock           = 62500,
    .bpp                = 16,
    .xres               = 240,
    .yres               = 320,

    .hsync_len          = 9,
    .vsync_len          = 2,
    .left_margin        = 9,
    .upper_margin       = 2,
    .right_margin       = 27,
    .lower_margin       = 4,

    .pcr                = PCR_TFT | PCR_COLOR | PCR_PBSIZ_8 | PCR_BPIX_16 | PCR_CLKPOL | 
                          PCR_SCLKIDLE | PCR_SCLK_SEL | PCR_PCD(9),
    .dmacr              = DEFAULT_DMA_SETTINGS,
    .pwmr               = 0,    // No contrast management
    .lcd_power          = apf9328_lcd_power,
};
#else
#error Please define a imxfb_mach_info structure with your LCD parameters here
#endif 

/*
 * Configure custom GPIOs needed by LCDs
 */
static void imx_fb_set_gpios(void)
{

#ifdef CONFIG_FB_IMX_OPTREX_F51900_CSTN
    DR(LCD_PORT) |= (1 << OPTREX_F51900_POWER_DOWN); // Initializes it High
    imx_gpio_mode( GPIO_PORTD | OPTREX_F51900_POWER_DOWN | GPIO_OUT | GPIO_DR );
#else
    DR(LCD_PORT) &= ~(1 << CONTRAST_LINE); // Initializes it low
    // GPIO Function for CONTRAST pin (changed in imxfb if contrast is activated)
    imx_gpio_mode( GPIO_PORTD | CONTRAST_LINE | GPIO_OUT | GPIO_GIUS | GPIO_DR );
#endif

#ifdef CONFIG_FB_IMX_SHARP_LQ043_TFT 
    // ACD_OE (SHARP_LQ043_POWER_DOWN) used as power down signal
    DR(LCD_PORT) &= ~(1 << SHARP_LQ043_POWER_DOWN); // Initializes it Low
    imx_gpio_mode( GPIO_PORTD | SHARP_LQ043_POWER_DOWN | GPIO_OUT | GPIO_GIUS | GPIO_DR ); 
#else
    // otherwise use ACD_OE as standard LCD controller signal
    imx_gpio_mode(PD12_PF_ACD_OE);
#endif // CONFIG_FB_IMX_SHARP_LQ043_TFT
}
#endif // CONFIG_FB_IMX

#endif // APF9328_LCD_CONFIG_H
