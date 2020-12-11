#ifndef __PV_PWM_H__
#define __PV_PWM_H__

#include <linux/ioctl.h>

#define PWM_PERIOD_SET _IOW('A', 1, unsigned long)
#define PWM_DUTY_SET _IOW('A', 2, unsigned long)
#define PWM_START _IOW('A', 3, unsigned long)

#endif
