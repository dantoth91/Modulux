/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
                 2011,2012 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _BOARD_H_
#define _BOARD_H_

/*
 * Setup for STMicroelectronics STM32F4-Discovery board.
 */

/*
 * Board identifier.
 */
#define BOARD_Modulux
#define BOARD_NAME                  "Modulux"


/*
 * Board oscillators-related settings.
 * NOTE: LSE not fitted.
 */
#if !defined(STM32_LSECLK)
#define STM32_LSECLK                0
#endif

#if !defined(STM32_HSECLK)
#define STM32_HSECLK                8000000
#endif


/*
 * Board voltages.
 * Required for performance limits calculation.
 */
#define STM32_VDD                   300

/*
 * MCU type as defined in the ST header file stm32f4xx.h.
 */
#define STM32F40_41xxx

/*
 * IO pins assignments.
 */
#define GPIOA_BUT1                  0
#define GPIOA_BuT2                  1
#define GPIOA_BUT3                  2
#define GPIOA_BUT4                  3
#define GPIOA_UBAT_DIV              4
#define GPIOA_PO5                   6
#define GPIOA_PO6                   7
#define GPIOA_USB_VBUS              9
#define GPIOA_USB_ID                10
#define GPIOA_USB_DM                11
#define GPIOA_USB_DP                12
#define GPIOA_J_TMS                 13
#define GPIOA_J_TCK                 14
#define GPIOA_J_TDI                 15

#define GPIOB_3V3_LOAD              0
#define GPIOB_X_5V_LOAD             1
#define GPIOB_J_TDO                 3
#define GPIOB_J_RST                 4
#define GPIOB_TXD1                  6
#define GPIOB_RXD1                  7
#define GPIOB_I2C1_C                8
#define GPIOB_I2C1_D                9

#define GPIOC_AIN1                  0
#define GPIOC_AIN2                  1
#define GPIOC_AIN3                  2
#define GPIOC_AIN4                  3
#define GPIOC_TXD6                  6
#define GPIOC_RXD6                  7
#define GPIOC_SD_D0                 8
#define GPIOC_SD_D1                 9
#define GPIOC_SD_D2                 10
#define GPIOC_SD_D3                 11
#define GPIOC_SD_CLK                12

#define GPIOD_CAN_RX                0
#define GPIOD_CAN_TX                1
#define GPIOD_SD_CMD                2
#define GPIOD_POX_ST                3
#define GPIOD_X_5V_EN               4
#define GPIOD_CARD_IN               5
#define GPIOD_LS_1                  12
#define GPIOD_LS_2                  13
#define GPIOD_LS_3                  14
#define GPIOD_LS_4                  15

#define GPIOE_ONEWIRE1              0
#define GPIOE_ONEWIRE2              1
#define GPIOE_LD1                   2
#define GPIOE_LD2                   3
#define GPIOE_PO1                   9
#define GPIOE_PO2                   11
#define GPIOE_PO3                   13
#define GPIOE_PO4                   14

#define GPIOH_OSC_IN                0
#define GPIOH_OSC_OUT               1

/*
 * I/O ports initial setup, this configuration is established soon after reset
 * in the initialization code.
 * Please refer to the STM32 Reference Manual for details.
 */
#define PIN_MODE_INPUT(n)           (0 << ((n) * 2))
#define PIN_MODE_OUTPUT(n)          (1 << ((n) * 2))
#define PIN_MODE_ALTERNATE(n)       (2 << ((n) * 2))
#define PIN_MODE_ANALOG(n)          (3 << ((n) * 2))
#define PIN_OTYPE_PUSHPULL(n)       (0 << (n))
#define PIN_OTYPE_OPENDRAIN(n)      (1 << (n))
#define PIN_OSPEED_2M(n)            (0 << ((n) * 2))
#define PIN_OSPEED_25M(n)           (1 << ((n) * 2))
#define PIN_OSPEED_50M(n)           (2 << ((n) * 2))
#define PIN_OSPEED_100M(n)          (3 << ((n) * 2))
#define PIN_PUDR_FLOATING(n)        (0 << ((n) * 2))
#define PIN_PUDR_PULLUP(n)          (1 << ((n) * 2))
#define PIN_PUDR_PULLDOWN(n)        (2 << ((n) * 2))
#define PIN_AFIO_AF(n, v)           ((v##U) << ((n % 8) * 4))

/*
 * Port A setup.
 * All input with pull-up except:
 */
#define VAL_GPIOA_MODER             0x00000000
#define VAL_GPIOA_OTYPER            0x00000000
#define VAL_GPIOA_OSPEEDR           0xFFFFFFFF
#define VAL_GPIOA_PUPDR             0xFFFFFFFF
#define VAL_GPIOA_ODR               0xFFFFFFFF
#define VAL_GPIOA_AFRL              0x00000000
#define VAL_GPIOA_AFRH              0x00000000

/*
 * Port B setup.
 * All input with pull-up except:
 * PB6 - Serial1 TX     (alternate 7)
 * PB7 - Serial1 RX     (alternate 7)
 */
#define VAL_GPIOB_MODER             (PIN_MODE_ALTERNATE(GPIOB_TXD1)       | \
                                     PIN_MODE_ALTERNATE(GPIOB_RXD1))
#define VAL_GPIOB_OTYPER            0x00000000
#define VAL_GPIOB_OSPEEDR           0xFFFFFFFF
#define VAL_GPIOB_PUPDR             0xFFFFFFFF
#define VAL_GPIOB_ODR               0xFFFFFFFF
#define VAL_GPIOB_AFRL              (PIN_AFIO_AF(GPIOB_TXD1, 7)           | \
                                     PIN_AFIO_AF(GPIOB_RXD1, 7))
#define VAL_GPIOB_AFRH              0x00000000

/*
 * Port C setup.
 * All input with pull-up except:
 * PB6 - Serial1 TX     (alternate 7)
 * PB7 - Serial1 RX     (alternate 7)
 * 
 */
#define VAL_GPIOC_MODER             (PIN_MODE_ALTERNATE(GPIOC_TXD6)       | \
                                     PIN_MODE_ALTERNATE(GPIOC_RXD6))
#define VAL_GPIOC_OTYPER            0x00000000
#define VAL_GPIOC_OSPEEDR           0xFFFFFFFF
#define VAL_GPIOC_PUPDR             0xFFFFFFFF
#define VAL_GPIOC_ODR               0xFFFFFFFF
#define VAL_GPIOC_AFRL              (PIN_AFIO_AF(GPIOB_TXD1, 8)           | \
                                     PIN_AFIO_AF(GPIOB_RXD1, 8))
#define VAL_GPIOC_AFRH              0x00000000

/*
 * Port D setup.
 * All input with pull-up except:
 */
#define VAL_GPIOD_MODER             (PIN_MODE_ALTERNATE(GPIOD_CAN_RX)     | \
                                     PIN_MODE_ALTERNATE(GPIOD_CAN_TX)      | \
                                     PIN_MODE_OUTPUT(GPIOD_X_5V_EN))
#define VAL_GPIOD_OTYPER            0x00000000
#define VAL_GPIOD_OSPEEDR           0xFFFFFFFF
#define VAL_GPIOD_PUPDR             0xFFFFFFFF
#define VAL_GPIOD_ODR               0xFFFFFFFF
#define VAL_GPIOD_AFRL              (PIN_AFIO_AF(GPIOD_CAN_RX, 9)          | \
                                     PIN_AFIO_AF(GPIOD_CAN_TX, 9))
#define VAL_GPIOD_AFRH              0x00000000

/*
 * Port E setup.
 * All input with pull-up except:
 */
#define VAL_GPIOE_MODER             (PIN_MODE_OUTPUT(GPIOE_LD1)             | \
                                     PIN_MODE_OUTPUT(GPIOE_LD2)             | \
                                     PIN_MODE_OUTPUT(GPIOE_PO1)             | \
                                     PIN_MODE_ALTERNATE(GPIOE_PO2)             | \
                                     PIN_MODE_OUTPUT(GPIOE_PO3)             | \
                                     PIN_MODE_OUTPUT(GPIOE_PO4))
#define VAL_GPIOE_OTYPER            0x00000000
#define VAL_GPIOE_OSPEEDR           0xFFFFFFFF
#define VAL_GPIOE_PUPDR             0xFFFFFFFF
#define VAL_GPIOE_ODR               0xFFFFFFFF
#define VAL_GPIOE_AFRL              0x00000000
#define VAL_GPIOE_AFRH             (PIN_AFIO_AF(GPIOE_PO2, 1 ))

/*
 * Port F setup.
 * All input with pull-up except:
 */
#define VAL_GPIOF_MODER             0x00000000
#define VAL_GPIOF_OTYPER            0x00000000
#define VAL_GPIOF_OSPEEDR           0xFFFFFFFF
#define VAL_GPIOF_PUPDR             0xFFFFFFFF
#define VAL_GPIOF_ODR               0xFFFFFFFF
#define VAL_GPIOF_AFRL              0x00000000
#define VAL_GPIOF_AFRH              0x00000000

/*
 * Port G setup.
 * All input with pull-up except:
 */
#define VAL_GPIOG_MODER             0x00000000
#define VAL_GPIOG_OTYPER            0x00000000
#define VAL_GPIOG_OSPEEDR           0xFFFFFFFF
#define VAL_GPIOG_PUPDR             0xFFFFFFFF
#define VAL_GPIOG_ODR               0xFFFFFFFF
#define VAL_GPIOG_AFRL              0x00000000
#define VAL_GPIOG_AFRH              0x00000000

/*
 * Port H setup.
 * All input with pull-up except:
 * PH0 - OSC_IN         (input floating).
 * PH1 - OSC_OUT        (input floating).
 */
#define VAL_GPIOH_MODER             0x00000000
#define VAL_GPIOH_OTYPER            0x00000000
#define VAL_GPIOH_OSPEEDR           0xFFFFFFFF
#define VAL_GPIOH_PUPDR             (~(PIN_PUDR_FLOATING(0)  | \
                                       PIN_PUDR_FLOATING(1)))
#define VAL_GPIOH_ODR               0xFFFFFFFF
#define VAL_GPIOH_AFRL              0x00000000
#define VAL_GPIOH_AFRH              0x00000000

/*
 * Port I setup.
 * All input with pull-up except:
 */
#define VAL_GPIOI_MODER             0x00000000
#define VAL_GPIOI_OTYPER            0x00000000
#define VAL_GPIOI_OSPEEDR           0xFFFFFFFF
#define VAL_GPIOI_PUPDR             0xFFFFFFFF
#define VAL_GPIOI_ODR               0xFFFFFFFF
#define VAL_GPIOI_AFRL              0x00000000
#define VAL_GPIOI_AFRH              0x00000000

#if !defined(_FROM_ASM_)
#ifdef __cplusplus
extern "C" {
#endif
  void boardInit(void);
#ifdef __cplusplus
}
#endif
#endif /* _FROM_ASM_ */

#endif /* _BOARD_H_ */
