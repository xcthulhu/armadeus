/*
**    THE ARMadeus Systems
**
**    Copyright (C) 2009  The armadeus systems team
**    Fabien Marteau <fabien.marteau@armadeus.com>
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public
** License along with this library; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef __ASAPF27PWM_H__
#define __ASAPF27PWM_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/* number of pwm under system */
#define NUMBER_OF_PWM 2

struct as_pwm_device {
    int fileFrequency;
    int filePeriod;
    int fileDuty;
    int fileActive;
};

/** @brief Open pwm
 *
 * @param aPwmNumber the pwm number
 *
 * @return pointer to device structure, NULL if error
 */
struct as_pwm_device *as_pwm_open(int aPwmNumber);

/** @brief set frequency
 *
 * @param aDev pointer to device structure
 * @param aFrequency frequency in Hz
 *
 * @return negative value on error
 */
int32_t as_pwm_set_frequency(struct as_pwm_device *aDev, int aFrequency);

/** @brief get frequency
 *
 * @param aDev pointer to device structure
 *
 * @return frequency value in Hz, negative value on error
 */
int32_t as_pwm_get_frequency(struct as_pwm_device *aDev);

/** @bief set period
 *
 * @param aDev pointer to device structure
 * @param aPeriod period in uS
 *
 * @return negative value on error
 */
int32_t as_pwm_set_period(struct as_pwm_device *aDev, int aPeriod);

/** @brief get period value
 *
 * @param aDev pointer to device structure
 *
 * @return period value in uS, negative value on error.
 */
int32_t as_pwm_get_period(struct as_pwm_device *aDev);

/** @brief set duty
 *
 * @param aDev pointer to device structure
 * @param aDuty duty in 0.1%
 *
 * @return negative value on error
 */
int32_t as_pwm_set_duty(struct as_pwm_device *aDev, int aDuty);

/** @brief get duty
 *
 * @param aDev pointer to device structure
 *
 * @return duty value in 0.1%, negative value on error
 */
int32_t  as_pwm_get_duty(struct as_pwm_device *aDev);

/** @brief set state of pwm
 *
 * @param aDev pointer to device structure
 * @param aEnable 0:disable, 1:enable
 *
 * @return negative value on error
 */
int32_t as_pwm_set_state(struct as_pwm_device *aDev, int aEnable);

/** @brief get state of pwm
 *
 * @param aDev pointer to device structure
 *
 * @return 0:disabled, 1:enabled, negative value on error
 */
int32_t as_pwm_get_state(struct as_pwm_device *aDev);

/** @brief close pwm
 *
 * @param aDev pointer structure to device
 *
 * @return negative value on error
 */
int32_t as_pwm_close(struct as_pwm_device *aDev);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* __ASAPF27PWM_H__ */
