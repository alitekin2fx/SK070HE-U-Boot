// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2012
 * Altera Corporation <www.altera.com>
 */

#include <common.h>
#include <clk.h>
#include <asm-generic/io.h>
#include <dm.h>
#include <fdtdec.h>
#include <malloc.h>
#include <reset.h>
#include <spi.h>
#include <spi-mem.h>
#include <thermal.h>
#include <linux/errno.h>
#include <linux/sizes.h>
#include "cadence_qspi.h"

#define NSEC_PER_SEC			1000000000L

#define CQSPI_STIG_READ			0
#define CQSPI_STIG_WRITE		1
#define CQSPI_READ			2
#define CQSPI_WRITE			3

#define CQSPI_PHY_INIT_RD		1
#define CQSPI_PHY_MAX_RD		4
#define CQSPI_PHY_MAX_RX		63
#define CQSPI_PHY_MAX_TX		63
#define CQSPI_PHY_LOW_RX_BOUND		15
#define CQSPI_PHY_HIGH_RX_BOUND		25
#define CQSPI_PHY_LOW_TX_BOUND		32
#define CQSPI_PHY_HIGH_TX_BOUND		48
#define CQSPI_PHY_TX_LOOKUP_LOW_BOUND	24
#define CQSPI_PHY_TX_LOOKUP_HIGH_BOUND	38

#define CQSPI_PHY_DEFAULT_TEMP		45
#define CQSPI_PHY_MIN_TEMP		-45
#define CQSPI_PHY_MAX_TEMP		135
#define CQSPI_PHY_MID_TEMP		(CQSPI_PHY_MIN_TEMP +	\
					 ((CQSPI_PHY_MAX_TEMP - CQSPI_PHY_MIN_TEMP) / 2))

#ifdef CONFIG_CADENCE_QSPI_PHY
static const u8 phy_tuning_pattern[] = {
0xFE, 0xFF, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xFE, 0xFE, 0x01, 0x01,
0x01, 0x01, 0x00, 0x00, 0xFE, 0xFE, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,
0x00, 0xFE, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFE, 0xFE, 0xFF, 0x01,
0x01, 0x01, 0x01, 0x01, 0xFE, 0x00, 0xFE, 0xFE, 0x01, 0x01, 0x01, 0x01, 0xFE,
0x00, 0xFE, 0xFE, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x00, 0xFE, 0xFE,
0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x00, 0xFE, 0xFE, 0xFF, 0x01, 0x01, 0x01, 0x01,
0x01, 0x00, 0xFE, 0xFE, 0xFE, 0x01, 0x01, 0x01, 0x01, 0x00, 0xFE, 0xFE, 0xFE,
0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFE, 0xFE, 0xFE, 0xFF, 0xFF, 0xFF,
0xFF, 0x00, 0xFE, 0xFE, 0xFE, 0xFF, 0x01, 0x01, 0x01, 0x01, 0x01, 0xFE, 0xFE,
0xFE, 0xFE, 0x01, 0x01, 0x01, 0x01, 0xFE, 0xFE, 0xFE, 0xFE, 0x01,
};

struct phy_setting {
	u8	rx;
	u8	tx;
	u8	read_delay;
};

static int cadence_spi_get_temp(int *temp)
{
	struct udevice *dev;
	int ret;

	ret = uclass_first_device_err(UCLASS_THERMAL, &dev);
	if (ret)
		return ret;

	ret = thermal_get_temp(dev, temp);
	if (ret)
		return ret;

	/* The temperature we get is in milicelsius. Change it to Celsius. */
	*temp /= 1000;

	return 0;
}

static void cadence_spi_phy_apply_setting(struct cadence_spi_platdata *plat,
					  struct phy_setting *phy)
{
	cadence_qspi_apb_set_rx_dll(plat->regbase, phy->rx);
	cadence_qspi_apb_set_tx_dll(plat->regbase, phy->tx);
	plat->phy_read_delay = phy->read_delay;
}

static int cadence_spi_phy_check_pattern(struct cadence_spi_platdata *plat,
					 struct spi_slave *spi)
{
	struct spi_mem_op op = plat->phy_read_op;
	u8 *read_data;
	int ret;

	read_data = kmalloc(ARRAY_SIZE(phy_tuning_pattern), 0);
	if (!read_data)
		return -ENOMEM;

	op.data.buf.in = read_data;
	op.addr.val = plat->phy_pattern_start;
	op.data.nbytes = ARRAY_SIZE(phy_tuning_pattern);

	ret = spi_mem_exec_op(spi, &op);
	if (ret)
		goto out;

	if (memcmp(read_data, phy_tuning_pattern,
		   ARRAY_SIZE(phy_tuning_pattern))) {
		ret = -EAGAIN;
		goto out;
	}

	ret = 0;
out:
	kfree(read_data);
	return ret;
}

static int cadence_spi_find_rx_low(struct cadence_spi_platdata *plat,
				   struct spi_slave *spi,
				   struct phy_setting *phy)
{
	int ret;

	do {
		phy->rx = 0;
		do {
			cadence_spi_phy_apply_setting(plat, phy);
			ret = cadence_spi_phy_check_pattern(plat, spi);
			if (!ret)
				return 0;

			phy->rx++;
		} while (phy->rx <= CQSPI_PHY_LOW_RX_BOUND);

		phy->read_delay++;
	} while (phy->read_delay <= CQSPI_PHY_MAX_RD);

	debug("Unable to find RX low\n");
	return -ENOENT;
}

static int cadence_spi_find_rx_high(struct cadence_spi_platdata *plat,
				    struct spi_slave *spi,
				    struct phy_setting *phy)
{
	int ret;

	do {
		phy->rx = CQSPI_PHY_MAX_RX;
		do {
			cadence_spi_phy_apply_setting(plat, phy);
			ret = cadence_spi_phy_check_pattern(plat, spi);
			if (!ret)
				return 0;

			phy->rx--;
		} while (phy->rx >= CQSPI_PHY_HIGH_RX_BOUND);

		phy->read_delay++;
	} while (phy->read_delay <= CQSPI_PHY_MAX_RD);

	debug("Unable to find RX high\n");
	return -ENOENT;
}

static int cadence_spi_find_tx_low(struct cadence_spi_platdata *plat,
				   struct spi_slave *spi,
				   struct phy_setting *phy)
{
	int ret;

	do {
		phy->tx = 0;
		do {
			cadence_spi_phy_apply_setting(plat, phy);
			ret = cadence_spi_phy_check_pattern(plat, spi);
			if (!ret)
				return 0;

			phy->tx++;
		} while (phy->tx <= CQSPI_PHY_LOW_TX_BOUND);

		phy->read_delay++;
	} while (phy->read_delay <= CQSPI_PHY_MAX_RD);

	debug("Unable to find TX low\n");
	return -ENOENT;
}

static int cadence_spi_find_tx_high(struct cadence_spi_platdata *plat,
				    struct spi_slave *spi,
				    struct phy_setting *phy)
{
	int ret;

	do {
		phy->tx = CQSPI_PHY_MAX_TX;
		do {
			cadence_spi_phy_apply_setting(plat, phy);
			ret = cadence_spi_phy_check_pattern(plat, spi);
			if (!ret)
				return 0;

			phy->tx--;
		} while (phy->tx >= CQSPI_PHY_HIGH_TX_BOUND);

		phy->read_delay++;
	} while (phy->read_delay <= CQSPI_PHY_MAX_RD);

	debug("Unable to find TX high\n");
	return -ENOENT;
}

static int cadence_spi_phy_find_gaplow(struct cadence_spi_platdata *plat,
				       struct spi_slave *spi,
				       struct phy_setting *bottomleft,
				       struct phy_setting *topright,
				       struct phy_setting *gaplow)
{
	struct phy_setting left, right, mid;
	int ret;

	left = *bottomleft;
	right = *topright;

	mid.tx = left.tx + ((right.tx - left.tx) / 2);
	mid.rx = left.rx + ((right.rx - left.rx) / 2);
	mid.read_delay = left.read_delay;

	do {
		cadence_spi_phy_apply_setting(plat, &mid);
		ret = cadence_spi_phy_check_pattern(plat, spi);
		if (ret) {
			/*
			 * Since we couldn't find the pattern, we need to go to
			 * the lower half.
			 */
			right.tx = mid.tx;
			right.rx = mid.rx;

			mid.tx = left.tx + ((mid.tx - left.tx) / 2);
			mid.rx = left.rx + ((mid.rx - left.rx) / 2);
		} else {
			/*
			 * Since we found the pattern, we need to go the the
			 * upper half.
			 */
			left.tx = mid.tx;
			left.rx = mid.rx;

			mid.tx = mid.tx + ((right.tx - mid.tx) / 2);
			mid.rx = mid.rx + ((right.rx - mid.rx) / 2);
		}

	/* Break the loop if the window has closed. */
	} while ((right.tx - left.tx >= 2) && (right.rx - left.rx >= 2));

	*gaplow = mid;
	return 0;
}

static int cadence_spi_phy_find_gaphigh(struct cadence_spi_platdata *plat,
					struct spi_slave *spi,
					struct phy_setting *bottomleft,
					struct phy_setting *topright,
					struct phy_setting *gaphigh)
{
	struct phy_setting left, right, mid;
	int ret;

	left = *bottomleft;
	right = *topright;

	mid.tx = left.tx + ((right.tx - left.tx) / 2);
	mid.rx = left.rx + ((right.rx - left.rx) / 2);
	mid.read_delay = right.read_delay;

	do {
		cadence_spi_phy_apply_setting(plat, &mid);
		ret = cadence_spi_phy_check_pattern(plat, spi);
		if (ret) {
			/*
			 * Since we couldn't find the pattern, we need to go the
			 * the upper half.
			 */
			left.tx = mid.tx;
			left.rx = mid.rx;

			mid.tx = mid.tx + ((right.tx - mid.tx) / 2);
			mid.rx = mid.rx + ((right.rx - mid.rx) / 2);
		} else {
			/*
			 * Since we found the pattern, we need to go to the
			 * lower half.
			 */
			right.tx = mid.tx;
			right.rx = mid.rx;

			mid.tx = left.tx + ((mid.tx - left.tx) / 2);
			mid.rx = left.rx + ((mid.rx - left.rx) / 2);
		}

	/* Break the loop if the window has closed. */
	} while ((right.tx - left.tx >= 2) && (right.rx - left.rx >= 2));

	*gaphigh = mid;
	return 0;
}

static int cadence_spi_phy_calibrate(struct cadence_spi_platdata *plat,
				     struct spi_slave *spi)
{
	struct phy_setting rxlow, rxhigh, txlow, txhigh, temp;
	struct phy_setting bottomleft, topright, searchpoint, gaplow, gaphigh;
	int ret, tmp;

	plat->use_phy = true;

	/* Look for RX boundaries at lower TX range. */
	rxlow.tx = plat->phy_tx_start;

	do {
		dev_dbg(dev, "Searching for rxlow on TX = %d\n", rxlow.tx);
		rxlow.read_delay = CQSPI_PHY_INIT_RD;
		ret = cadence_spi_find_rx_low(plat, spi, &rxlow);
	} while (ret && ++rxlow.tx <= CQSPI_PHY_TX_LOOKUP_LOW_BOUND);
	if (ret)
		goto out;
	dev_dbg(dev, "rxlow: RX: %d TX: %d RD: %d\n", rxlow.rx, rxlow.tx,
		rxlow.read_delay);

	rxhigh.tx = rxlow.tx;
	rxhigh.read_delay = rxlow.read_delay;
	cadence_spi_find_rx_high(plat, spi, &rxhigh);
	if (ret)
		goto out;
	dev_dbg(dev, "rxhigh: RX: %d TX: %d RD: %d\n", rxhigh.rx, rxhigh.tx,
		rxhigh.read_delay);

	/*
	 * Check a different point if rxlow and rxhigh are on the same read
	 * delay. This avoids mistaking the failing region for an RX boundary.
	 */
	if (rxlow.read_delay == rxhigh.read_delay) {
		dev_dbg(dev,
			"rxlow and rxhigh at the same read delay.\n");

		/* Look for RX boundaries at upper TX range. */
		temp.tx = plat->phy_tx_end;

		do {
			dev_dbg(dev, "Searching for rxlow on TX = %d\n",
				temp.tx);
			temp.read_delay = CQSPI_PHY_INIT_RD;
			ret = cadence_spi_find_rx_low(plat, spi, &temp);
		} while (ret && --temp.tx >= CQSPI_PHY_TX_LOOKUP_HIGH_BOUND);
		if (ret)
			goto out;
		dev_dbg(dev, "rxlow: RX: %d TX: %d RD: %d\n", temp.rx, temp.tx,
			temp.read_delay);

		if (temp.rx < rxlow.rx) {
			rxlow = temp;
			dev_dbg(dev, "Updating rxlow to the one at TX = 48\n");
		}

		/* Find RX max. */
		ret = cadence_spi_find_rx_high(plat, spi, &temp);
		if (ret)
			goto out;
		dev_dbg(dev, "rxhigh: RX: %d TX: %d RD: %d\n", temp.rx, temp.tx,
			temp.read_delay);

		if (temp.rx < rxhigh.rx) {
			rxhigh = temp;
			dev_dbg(dev, "Updating rxhigh to the one at TX = 48\n");
		}
	}

	/* Look for TX boundaries at 1/4 of RX window. */
	txlow.rx = rxlow.rx + ((rxhigh.rx - rxlow.rx) / 4);
	txhigh.rx = txlow.rx;

	txlow.read_delay = CQSPI_PHY_INIT_RD;
	ret = cadence_spi_find_tx_low(plat, spi, &txlow);
	if (ret)
		goto out;
	dev_dbg(dev, "txlow: RX: %d TX: %d RD: %d\n", txlow.rx, txlow.tx,
		txlow.read_delay);

	txhigh.read_delay = txlow.read_delay;
	ret = cadence_spi_find_tx_high(plat, spi, &txhigh);
	if (ret)
		goto out;
	dev_dbg(dev, "txhigh: RX: %d TX: %d RD: %d\n", txhigh.rx, txhigh.tx,
		txhigh.read_delay);

	/*
	 * Check a different point if txlow and txhigh are on the same read
	 * delay. This avoids mistaking the failing region for an TX boundary.
	 */
	if (txlow.read_delay == txhigh.read_delay) {
		/* Look for TX boundaries at 3/4 of RX window. */
		temp.rx = rxlow.rx + (3 * (rxhigh.rx - rxlow.rx) / 4);
		temp.read_delay = CQSPI_PHY_INIT_RD;
		dev_dbg(dev,
			"txlow and txhigh at the same read delay. Searching at RX = %d\n",
			temp.rx);

		ret = cadence_spi_find_tx_low(plat, spi, &temp);
		if (ret)
			goto out;
		dev_dbg(dev, "txlow: RX: %d TX: %d RD: %d\n", temp.rx, temp.tx,
			temp.read_delay);

		if (temp.tx < txlow.tx) {
			txlow = temp;
			dev_dbg(dev, "Updating txlow with the one at RX = %d\n",
				txlow.rx);
		}

		ret = cadence_spi_find_tx_high(plat, spi, &temp);
		if (ret)
			goto out;
		dev_dbg(dev, "txhigh: RX: %d TX: %d RD: %d\n", temp.rx, temp.tx,
			temp.read_delay);

		if (temp.tx < txhigh.tx) {
			txhigh = temp;
			dev_dbg(dev, "Updating txhigh with the one at RX = %d\n",
				txhigh.rx);
		}
	}

	/*
	 * Set bottom left and top right corners. These are theoretical
	 * corners. They may not actually be "good" points. But the longest
	 * diagonal will be between these corners.
	 */
	bottomleft.tx = txlow.tx;
	bottomleft.rx = rxlow.rx;
	if (txlow.read_delay <= rxlow.read_delay)
		bottomleft.read_delay = txlow.read_delay;
	else
		bottomleft.read_delay = rxlow.read_delay;

	temp = bottomleft;
	temp.tx += 4;
	temp.rx += 4;
	cadence_spi_phy_apply_setting(plat, &temp);
	ret = cadence_spi_phy_check_pattern(plat, spi);
	if (ret) {
		temp.read_delay--;
		cadence_spi_phy_apply_setting(plat, &temp);
		ret = cadence_spi_phy_check_pattern(plat, spi);
	}

	if (!ret)
		bottomleft.read_delay = temp.read_delay;

	topright.tx = txhigh.tx;
	topright.rx = rxhigh.rx;
	if (txhigh.read_delay >= rxhigh.read_delay)
		topright.read_delay = txhigh.read_delay;
	else
		topright.read_delay = rxhigh.read_delay;

	temp = topright;
	temp.tx -= 4;
	temp.rx -= 4;
	cadence_spi_phy_apply_setting(plat, &temp);
	ret = cadence_spi_phy_check_pattern(plat, spi);
	if (ret) {
		temp.read_delay++;
		cadence_spi_phy_apply_setting(plat, &temp);
		ret = cadence_spi_phy_check_pattern(plat, spi);
	}

	if (!ret)
		topright.read_delay = temp.read_delay;

	dev_dbg(dev, "topright: RX: %d TX: %d RD: %d\n", topright.rx,
		topright.tx, topright.read_delay);
	dev_dbg(dev, "bottomleft: RX: %d TX: %d RD: %d\n", bottomleft.rx,
		bottomleft.tx, bottomleft.read_delay);

	ret = cadence_spi_phy_find_gaplow(plat, spi, &bottomleft, &topright,
					  &gaplow);
	if (ret)
		return ret;
	dev_dbg(dev, "gaplow: RX: %d TX: %d RD: %d\n", gaplow.rx, gaplow.tx,
		gaplow.read_delay);

	if (bottomleft.read_delay == topright.read_delay) {
		/*
		 * If there is only one passing region, it means that the "true"
		 * topright is too small to find, so the start of the failing
		 * region is a good approximation. Put the tuning point in the
		 * middle and adjust for temperature.
		 */
		topright = gaplow;
		searchpoint.read_delay = bottomleft.read_delay;
		searchpoint.tx = bottomleft.tx +
				 ((topright.tx - bottomleft.tx) / 2);
		searchpoint.rx = bottomleft.rx +
				 ((topright.rx - bottomleft.rx) / 2);

		ret = cadence_spi_get_temp(&tmp);
		if (ret) {
			/*
			 * Assume room temperature if we couldn't get it from
			 * the thermal sensor.
			 */
			printf("WARN: Unable to get temperature. Assuming room temperature\n");
			tmp = CQSPI_PHY_DEFAULT_TEMP;
		}

		if (tmp < CQSPI_PHY_MIN_TEMP || tmp > CQSPI_PHY_MAX_TEMP) {
			printf("ERROR: temperature outside operating range: %dC\n",
			       tmp);
			return -EINVAL;
		}

		/* Avoid a divide-by-zero. */
		if (tmp == CQSPI_PHY_MID_TEMP)
			tmp++;
		dev_dbg(dev, "Temperature: %dC\n", tmp);

		searchpoint.tx += (topright.tx - bottomleft.tx) /
				  (330 / (tmp - CQSPI_PHY_MID_TEMP));
		searchpoint.rx += (topright.rx - bottomleft.rx) /
				  (330 / (tmp - CQSPI_PHY_MID_TEMP));
	} else {
		/*
		 * If there are two passing regions, find the start and end of
		 * the second one.
		 */
		ret = cadence_spi_phy_find_gaphigh(plat, spi, &bottomleft,
						   &topright, &gaphigh);
		if (ret)
			return ret;
		dev_dbg(dev, "gaphigh: RX: %d TX: %d RD: %d\n", gaphigh.rx,
			gaphigh.tx, gaphigh.read_delay);

		/*
		 * Place the final tuning point in the corner furthest from the
		 * failing region but leave some margin for temperature changes.
		 */
		if ((abs(gaplow.tx - bottomleft.tx) +
		     abs(gaplow.rx - bottomleft.rx)) <
		    (abs(gaphigh.tx - topright.tx) +
		     abs(gaphigh.rx - topright.rx))) {
			searchpoint = topright;
			searchpoint.tx -= 16;
			searchpoint.rx -= (16 * (topright.rx - bottomleft.rx)) /
					   (topright.tx - bottomleft.tx);
		} else {
			searchpoint = bottomleft;
			searchpoint.tx += 16;
			searchpoint.rx += (16 * (topright.rx - bottomleft.rx)) /
					   (topright.tx - bottomleft.tx);
		}
	}

	/* Set the final PHY settings we found. */
	cadence_spi_phy_apply_setting(plat, &searchpoint);
	dev_dbg(dev, "Final tuning point: RX: %d TX: %d RD: %d\n",
		searchpoint.rx, searchpoint.tx, searchpoint.read_delay);

	ret = cadence_spi_phy_check_pattern(plat, spi);
	if (ret) {
		debug("Failed to find pattern at final calibration point\n");
		ret = -EINVAL;
		goto out;
	}

	ret = 0;
	plat->phy_read_delay = searchpoint.read_delay;
out:
	if (ret)
		plat->use_phy = false;
	return ret;
}
#else
static int cadence_spi_phy_calibrate(struct cadence_spi_platdata *plat,
				     struct spi_slave *spi)
{
	plat->use_phy = false;
	return -ENOSYS;
}
#endif /* CONFIG_CADENCE_QSPI_PHY */

static int cadence_spi_write_speed(struct udevice *bus, uint hz)
{
	struct cadence_spi_platdata *plat = bus->platdata;
	struct cadence_spi_priv *priv = dev_get_priv(bus);

	cadence_qspi_apb_config_baudrate_div(priv->regbase,
					     plat->ref_clk_hz, hz);

	/* Reconfigure delay timing if speed is changed. */
	cadence_qspi_apb_delay(priv->regbase, plat->ref_clk_hz, hz,
			       plat->tshsl_ns, plat->tsd2d_ns,
			       plat->tchsh_ns, plat->tslch_ns);

	return 0;
}

static int cadence_spi_read_id(struct cadence_spi_platdata *plat, u8 len,
			       u8 *idcode)
{
	struct spi_mem_op op = SPI_MEM_OP(SPI_MEM_OP_CMD(0x9F, 1),
					  SPI_MEM_OP_NO_ADDR,
					  SPI_MEM_OP_NO_DUMMY,
					  SPI_MEM_OP_DATA_IN(len, idcode, 1));

	return cadence_qspi_apb_command_read(plat, &op);
}

/* Calibration sequence to determine the read data capture delay register */
static int spi_calibration(struct udevice *bus, uint hz)
{
	struct cadence_spi_priv *priv = dev_get_priv(bus);
	struct cadence_spi_platdata *plat = bus->platdata;
	void *base = priv->regbase;
	unsigned int idcode = 0, temp = 0;
	int err = 0, i, range_lo = -1, range_hi = -1;

	/* start with slowest clock (1 MHz) */
	cadence_spi_write_speed(bus, 1000000);

	/* configure the read data capture delay register to 0 */
	cadence_qspi_apb_readdata_capture(base, 1, 0);

	/* Enable QSPI */
	cadence_qspi_apb_controller_enable(base);

	/* read the ID which will be our golden value */
	err = cadence_spi_read_id(plat, 3, (u8 *)&idcode);
	if (err) {
		puts("SF: Calibration failed (read)\n");
		return err;
	}

	/* use back the intended clock and find low range */
	cadence_spi_write_speed(bus, hz);
	for (i = 0; i < CQSPI_READ_CAPTURE_MAX_DELAY; i++) {
		/* Disable QSPI */
		cadence_qspi_apb_controller_disable(base);

		/* reconfigure the read data capture delay register */
		cadence_qspi_apb_readdata_capture(base, 1, i);

		/* Enable back QSPI */
		cadence_qspi_apb_controller_enable(base);

		/* issue a RDID to get the ID value */
		err = cadence_spi_read_id(plat, 3, (u8 *)&temp);
		if (err) {
			puts("SF: Calibration failed (read)\n");
			return err;
		}

		/* search for range lo */
		if (range_lo == -1 && temp == idcode) {
			range_lo = i;
			continue;
		}

		/* search for range hi */
		if (range_lo != -1 && temp != idcode) {
			range_hi = i - 1;
			break;
		}
		range_hi = i;
	}

	if (range_lo == -1) {
		puts("SF: Calibration failed (low range)\n");
		return err;
	}

	/* Disable QSPI for subsequent initialization */
	cadence_qspi_apb_controller_disable(base);

	/* configure the final value for read data capture delay register */
	cadence_qspi_apb_readdata_capture(base, 1, (range_hi + range_lo) / 2);
	debug("SF: Read data capture delay calibrated to %i (%i - %i)\n",
	      (range_hi + range_lo) / 2, range_lo, range_hi);

	/* just to ensure we do once only when speed or chip select change */
	priv->qspi_calibrated_hz = hz;
	priv->qspi_calibrated_cs = spi_chip_select(bus);

	return 0;
}

static int cadence_spi_set_speed(struct udevice *bus, uint hz)
{
	struct cadence_spi_platdata *plat = bus->platdata;
	struct cadence_spi_priv *priv = dev_get_priv(bus);
	int err;

	if (hz > plat->max_hz)
		hz = plat->max_hz;

	/* Disable QSPI */
	cadence_qspi_apb_controller_disable(priv->regbase);

	/*
	 * If we will use PHY calibration we don't need to run
	 * spi_calibration().
	 */
	if (plat->has_phy) {
		cadence_spi_write_speed(bus, hz);
		cadence_qspi_apb_readdata_capture(priv->regbase, 1,
						  plat->read_delay);
	} else if (priv->previous_hz != hz ||
		   priv->qspi_calibrated_hz != hz ||
		   priv->qspi_calibrated_cs != spi_chip_select(bus)) {
		/*
		 * Calibration required for different current SCLK speed,
		 * requested SCLK speed or chip select
		 */
		err = spi_calibration(bus, hz);
		if (err)
			return err;

		/* prevent calibration run when same as previous request */
		priv->previous_hz = hz;
	}

	/* Enable QSPI */
	cadence_qspi_apb_controller_enable(priv->regbase);

	debug("%s: speed=%d\n", __func__, hz);

	return 0;
}

static int cadence_spi_probe(struct udevice *bus)
{
	struct cadence_spi_platdata *plat = bus->platdata;
	struct cadence_spi_priv *priv = dev_get_priv(bus);
	struct clk clk;
	int ret;

	priv->regbase = plat->regbase;
	priv->ahbbase = plat->ahbbase;

	if (plat->ref_clk_hz == 0) {
		ret = clk_get_by_index(bus, 0, &clk);
		if (ret) {
#ifdef CONFIG_CQSPI_REF_CLK
			plat->ref_clk_hz = CONFIG_CQSPI_REF_CLK;
#else
			return ret;
#endif
		} else {
			plat->ref_clk_hz = clk_get_rate(&clk);
			clk_free(&clk);
			if (IS_ERR_VALUE(plat->ref_clk_hz))
				return plat->ref_clk_hz;
		}
	}

	ret = reset_get_bulk(bus, &priv->resets);
	if (ret)
		dev_warn(bus, "Can't get reset: %d\n", ret);
	else
		reset_deassert_bulk(&priv->resets);

	if (!priv->qspi_is_init) {
		cadence_qspi_apb_controller_init(plat);
		priv->qspi_is_init = 1;
	}

	plat->wr_delay = 50 * DIV_ROUND_UP(NSEC_PER_SEC, plat->ref_clk_hz);

	return 0;
}

static int cadence_spi_remove(struct udevice *dev)
{
	struct cadence_spi_priv *priv = dev_get_priv(dev);

	return reset_release_bulk(&priv->resets);
}

static int cadence_spi_set_mode(struct udevice *bus, uint mode)
{
	struct cadence_spi_platdata *plat = bus->platdata;
	struct cadence_spi_priv *priv = dev_get_priv(bus);

	/* Disable QSPI */
	cadence_qspi_apb_controller_disable(priv->regbase);

	/* Set SPI mode */
	cadence_qspi_apb_set_clk_mode(priv->regbase, mode);

	/* Enable Direct Access Controller */
	if (plat->use_dac_mode)
		cadence_qspi_apb_dac_mode_enable(priv->regbase);

	/* Enable QSPI */
	cadence_qspi_apb_controller_enable(priv->regbase);

	return 0;
}

static int cadence_spi_mem_exec_op(struct spi_slave *spi,
				   const struct spi_mem_op *op)
{
	struct udevice *bus = spi->dev->parent;
	struct cadence_spi_platdata *plat = bus->platdata;
	struct cadence_spi_priv *priv = dev_get_priv(bus);
	void *base = priv->regbase;
	int err = 0;
	u32 mode;

	/* Set Chip select */
	cadence_qspi_apb_chipselect(base, spi_chip_select(spi->dev),
				    plat->is_decoded_cs);

	if (op->data.dir == SPI_MEM_DATA_IN && op->data.buf.in) {
		if (!op->addr.nbytes)
			mode = CQSPI_STIG_READ;
		else
			mode = CQSPI_READ;
	} else {
		if (!op->addr.nbytes || !op->data.buf.out)
			mode = CQSPI_STIG_WRITE;
		else
			mode = CQSPI_WRITE;
	}

	switch (mode) {
	case CQSPI_STIG_READ:
		err = cadence_qspi_apb_command_read_setup(plat, op);
		if (!err)
			err = cadence_qspi_apb_command_read(plat, op);
		break;
	case CQSPI_STIG_WRITE:
		err = cadence_qspi_apb_command_write_setup(plat, op);
		if (!err)
			err = cadence_qspi_apb_command_write(plat, op);
		break;
	case CQSPI_READ:
		err = cadence_qspi_apb_read_setup(plat, op);
		if (!err)
			err = cadence_qspi_apb_read_execute(plat, op);
		break;
	case CQSPI_WRITE:
		err = cadence_qspi_apb_write_setup(plat, op);
		if (!err)
			err = cadence_qspi_apb_write_execute(plat, op);
		break;
	default:
		err = -1;
		break;
	}

	return err;
}

static int cadence_spi_check_buswidth_req(struct spi_slave *slave, u8 buswidth,
					  bool tx)
{
	u32 mode = slave->mode;

	switch (buswidth) {
	case 1:
		return 0;

	case 2:
		if ((tx && (mode & (SPI_TX_DUAL | SPI_TX_QUAD))) ||
		    (!tx && (mode & (SPI_RX_DUAL | SPI_RX_QUAD))))
			return 0;

		break;

	case 4:
		if ((tx && (mode & SPI_TX_QUAD)) ||
		    (!tx && (mode & SPI_RX_QUAD)))
			return 0;

		break;
	case 8:
		if ((tx && (mode & SPI_TX_OCTAL)) ||
		    (!tx && (mode & SPI_RX_OCTAL)))
			return 0;

		break;

	default:
		break;
	}

	return -ENOTSUPP;
}

static bool cadence_spi_mem_supports_op(struct spi_slave *slave,
					const struct spi_mem_op *op)
{
	bool all_true, all_false;

	if (cadence_spi_check_buswidth_req(slave, op->cmd.buswidth, true))
		return false;

	if (op->addr.nbytes &&
	    cadence_spi_check_buswidth_req(slave, op->addr.buswidth, true))
		return false;

	if (op->dummy.nbytes &&
	    cadence_spi_check_buswidth_req(slave, op->dummy.buswidth, true))
		return false;

	if (op->data.nbytes &&
	    cadence_spi_check_buswidth_req(slave, op->data.buswidth,
					   op->data.dir == SPI_MEM_DATA_OUT))
		return false;

	all_true = op->cmd.dtr && op->addr.dtr && op->dummy.dtr &&
		   op->data.dtr;
	all_false = !op->cmd.dtr && !op->addr.dtr && !op->dummy.dtr &&
		    !op->data.dtr;

	/* Mixed DTR modes not supported. */
	if (!(all_true || all_false))
		return false;

	/* DTR mode opcodes should be 2 bytes. */
	if (all_true && op->cmd.nbytes != 2)
		return false;

	return true;
}

static void cadence_spi_mem_phy_op(struct spi_slave *spi,
				   struct spi_mem_op *op)
{
	struct udevice *bus = spi->dev->parent;
	struct cadence_spi_platdata *plat = bus->platdata;
	int ret;

	if (!plat->has_phy)
		return;

	plat->phy_read_op = *op;

	ret = cadence_spi_phy_calibrate(plat, spi);
	if (ret)
		printf("WARN: PHY calibration failed: %d\n", ret);
}

static int cadence_spi_ofdata_to_platdata(struct udevice *bus)
{
	struct cadence_spi_platdata *plat = bus->platdata;
	ofnode subnode;
	const char *label;
	u32 start;

	plat->regbase = (void *)devfdt_get_addr_index(bus, 0);
	plat->ahbbase = (void *)devfdt_get_addr_size_index(bus, 1,
			&plat->ahbsize);
	plat->is_decoded_cs = dev_read_bool(bus, "cdns,is-decoded-cs");
	plat->fifo_depth = dev_read_u32_default(bus, "cdns,fifo-depth", 128);
	plat->fifo_width = dev_read_u32_default(bus, "cdns,fifo-width", 4);
	plat->trigger_address = dev_read_u32_default(bus,
						     "cdns,trigger-address",
						     0);
	/* Use DAC mode only when MMIO window is at least 8M wide */
	if (plat->ahbsize >= SZ_8M)
		plat->use_dac_mode = true;

	/* All other paramters are embedded in the child node */
	subnode = dev_read_first_subnode(bus);
	if (!ofnode_valid(subnode)) {
		printf("Error: subnode with SPI flash config missing!\n");
		return -ENODEV;
	}

	/* Use 500 KHz as a suitable default */
	plat->max_hz = ofnode_read_u32_default(subnode, "spi-max-frequency",
					       500000);

	/* Read other parameters from DT */
	plat->page_size = ofnode_read_u32_default(subnode, "page-size", 256);
	plat->block_size = ofnode_read_u32_default(subnode, "block-size", 16);
	plat->tshsl_ns = ofnode_read_u32_default(subnode, "cdns,tshsl-ns",
						 200);
	plat->tsd2d_ns = ofnode_read_u32_default(subnode, "cdns,tsd2d-ns",
						 255);
	plat->tchsh_ns = ofnode_read_u32_default(subnode, "cdns,tchsh-ns", 20);
	plat->tslch_ns = ofnode_read_u32_default(subnode, "cdns,tslch-ns", 20);
	/*
	 * Read delay should be an unsigned value but we use a signed integer
	 * so that negative values can indicate that the device tree did not
	 * specify any signed values and we need to perform the calibration
	 * sequence to find it out.
	 */
	plat->read_delay = ofnode_read_s32_default(subnode, "cdns,read-delay",
						   0);
	plat->has_phy = ofnode_read_bool(subnode, "cdns,phy-mode");

	plat->phy_tx_start = ofnode_read_u32_default(subnode,
						     "cdns,phy-tx-start",
						     16);
	plat->phy_tx_end = ofnode_read_u32_default(subnode,
						   "cdns,phy-tx-end",
						   48);

	/* Find the PHY tuning pattern partition. */
	subnode = ofnode_first_subnode(subnode);
	while (ofnode_valid(subnode)) {
		label = ofnode_read_string(subnode, "label");
		if (label && strcmp(label, "ospi.phypattern") == 0) {
			if (!ofnode_read_u32_array(subnode, "reg", &start, 1))
				plat->phy_pattern_start = start;
			break;
		}
		subnode = ofnode_next_subnode(subnode);
	}

	debug("%s: regbase=%p ahbbase=%p max-frequency=%d page-size=%d\n",
	      __func__, plat->regbase, plat->ahbbase, plat->max_hz,
	      plat->page_size);

	return 0;
}

static const struct spi_controller_mem_ops cadence_spi_mem_ops = {
	.exec_op = cadence_spi_mem_exec_op,
	.supports_op = cadence_spi_mem_supports_op,
	.set_calibration_read_op = cadence_spi_mem_phy_op,
};

static const struct dm_spi_ops cadence_spi_ops = {
	.set_speed	= cadence_spi_set_speed,
	.set_mode	= cadence_spi_set_mode,
	.mem_ops	= &cadence_spi_mem_ops,
	/*
	 * cs_info is not needed, since we require all chip selects to be
	 * in the device tree explicitly
	 */
};

static const struct udevice_id cadence_spi_ids[] = {
	{ .compatible = "cdns,qspi-nor" },
	{ .compatible = "ti,am654-ospi" },
	{ }
};

U_BOOT_DRIVER(cadence_spi) = {
	.name = "cadence_spi",
	.id = UCLASS_SPI,
	.of_match = cadence_spi_ids,
	.ops = &cadence_spi_ops,
	.ofdata_to_platdata = cadence_spi_ofdata_to_platdata,
	.platdata_auto_alloc_size = sizeof(struct cadence_spi_platdata),
	.priv_auto_alloc_size = sizeof(struct cadence_spi_priv),
	.probe = cadence_spi_probe,
	.remove = cadence_spi_remove,
	.flags = DM_FLAG_OS_PREPARE,
};
