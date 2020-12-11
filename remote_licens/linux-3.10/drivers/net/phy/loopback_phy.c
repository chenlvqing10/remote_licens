/*
 * loopback_phy.c - Driver for Loopback PHYs
 *
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <linux/module.h>
#include <linux/phy.h>

static int loopback_phy_config_init(struct phy_device *phydev)
{
	int val;
	u32 features;

	/* For now, I'll claim that the loopback phy driver supports
	 * all possible port types, but autonegotiation.
	 */
	features = (SUPPORTED_TP | SUPPORTED_MII | SUPPORTED_AUI |
		SUPPORTED_FIBRE | SUPPORTED_BNC);

	val = phy_read(phydev, MII_BMSR);
	if (val < 0)
		return val;

	if (val & BMSR_100FULL)
		features |= SUPPORTED_100baseT_Full;
	if (val & BMSR_100HALF)
		features |= SUPPORTED_100baseT_Half;
	if (val & BMSR_10FULL)
		features |= SUPPORTED_10baseT_Full;
	if (val & BMSR_10HALF)
		features |= SUPPORTED_10baseT_Half;

#if 0
	if (val & BMSR_ESTATEN) {
		val = phy_read(phydev, MII_ESTATUS);

		if (val < 0)
			return val;

		if (val & ESTATUS_1000_TFULL)
			features |= SUPPORTED_1000baseT_Full;
		if (val & ESTATUS_1000_THALF)
			features |= SUPPORTED_1000baseT_Half;
	}
#endif

	phydev->supported = features;
	phydev->advertising = features;
	phydev->autoneg = AUTONEG_DISABLE;

	phydev->speed = 100;
	phydev->duplex = 1;

	phydev->link = 1;
	phydev->pause = phydev->asym_pause = 0;

	return 0;
}

static int loopback_phy_setup_forced(struct phy_device *phydev)
{
	int ret, ctl = BMCR_LOOPBACK;

	phydev->link = 1;

	if (SPEED_1000 == phydev->speed)
		ctl |= BMCR_SPEED1000;
	else if (SPEED_100 == phydev->speed)
		ctl |= BMCR_SPEED100;

	if (DUPLEX_FULL == phydev->duplex)
		ctl |= BMCR_FULLDPLX;

	ret = phy_write(phydev, MII_BMCR, ctl);

	return ret;
}

static int loopback_phy_config_aneg(struct phy_device *phydev)
{
	if (AUTONEG_ENABLE != phydev->autoneg)
		return loopback_phy_setup_forced(phydev);

	return -EOPNOTSUPP;
}

static int loopback_phy_read_status(struct phy_device *phydev)
{
	int bmcr;

	phydev->link = 1;
	phydev->pause = phydev->asym_pause = 0;

	bmcr = phy_read(phydev, MII_BMCR);
	if (bmcr < 0)
		return bmcr;

	if (bmcr & BMCR_FULLDPLX)
		phydev->duplex = DUPLEX_FULL;
	else
		phydev->duplex = DUPLEX_HALF;

	if (bmcr & BMCR_SPEED1000)
		phydev->speed = SPEED_1000;
	else if (bmcr & BMCR_SPEED100)
		phydev->speed = SPEED_100;
	else
		phydev->speed = SPEED_10;

	return 0;
}

static int loopback_phy_match_device(struct phy_device *phydev)
{
	/* match all kinds of PHYs */
	return 1;
}

static struct phy_driver loopback_phy_driver = {
	.phy_id			= 0xffffffff,
	.phy_id_mask		= 0xffffffff,
	.name			= "Loopback PHY",
	.config_init		= loopback_phy_config_init,
	.features		= 0,
	.config_aneg		= loopback_phy_config_aneg,
	.read_status		= loopback_phy_read_status,
	.match_phy_device	= loopback_phy_match_device,
	.driver			= {.owner = THIS_MODULE,},
};

static int __init loopback_phy_init(void)
{
	return phy_driver_register(&loopback_phy_driver);
}

static void __exit loopback_phy_exit(void)
{
	phy_driver_unregister(&loopback_phy_driver);
}

module_init(loopback_phy_init);
module_exit(loopback_phy_exit);

MODULE_DESCRIPTION("Driver for Loopback PHYs");
MODULE_AUTHOR("lomboswer <lomboswer@lombotech.com>");
MODULE_LICENSE("GPL v2");
