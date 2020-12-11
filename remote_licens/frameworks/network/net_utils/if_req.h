/* if_req.h */

#ifndef __IF_REQ_H__
#define __IF_REQ_H__

/**
 * ifreq_init - init ifreq.
 * @dev_name: device name.
 * @ifr: pointer to ifreq struct.
 */
void ifreq_init(const char *name, struct ifreq *ifr);

/**
 * ifreq_set_flags - ifreq set flags.
 * @dev_name: device name.
 * @flags: flags to set.
 *
 * return 0 if success; otherwise failed.
 */
int ifreq_set_flags(const char *name, unsigned int flags);

/**
 * ifreq_clr_flags - ifreq clear flags.
 * @dev_name: device name.
 * @flags: flags to clear.
 *
 * return 0 if success; otherwise failed.
 */
int ifreq_clr_flags(const char *name, unsigned int flags);

/**
 * ifreq_get_flags - ifreq get flags.
 * @dev_name: device name.
 * @flags: the flags we get.
 *
 * return 0 if success; otherwise failed.
 */
int ifreq_get_flags(const char *name, unsigned int *flags);

#endif /* __IF_REQ_H__ */
