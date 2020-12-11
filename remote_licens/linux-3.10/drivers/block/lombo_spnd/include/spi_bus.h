#ifndef ___SPI__BUS_H__
#define ___SPI__BUS_H__

/* the low 8bit is for the nand chip ops status */
#define TRANS_STAUS_MASK	(0xFFFFFF00UL)
#define TRANS_WAIT_TIMEOUT	(0x00010000UL)
#define TRANS_BUS_ERR		(0x00020000UL)

#define SPI_TRANS_ACT_MASK	(0x0F)
#define SPI_TRANS_ASYNC_POLL	(0x10)
#define SPI_TRANS_TO_POLL	(0x20)

enum spi_trans_act {
	SPI_TRANS_NO_DATA,
	SPI_TRANS_CTL_READ,
	SPI_TRANS_CTL_PROG,
	SPI_TRANS_CTL_ERASE,
	SPI_TRANS_CTL_DIESEL,
	SPI_TRANS_READ,
	SPI_TRANS_WRITE,
};

enum spi_trans_type {
	SPI_BUS_TRANS_1_1_1,
	SPI_BUS_TRANS_1_1_2,
	SPI_BUS_TRANS_1_1_4,
	SPI_BUS_TRANS_1_2_2,
	SPI_BUS_TRANS_1_4_4,
};

#pragma pack(push, 1)
struct spi_bus_trans {
	u8	trans_act;
	u8	transtype;
	u8	rsvd0[2];
	u32	addr;
	u8	*buf;
	u32	bytes;
	u8	cmd;
	u8	addr_bits;
	u8	dummy_bits;
	u8	rsvd1;

	u8	poll_cmd;
	u8	poll_bytes;
	u8	rsvd2[2];
	u32	poll_addr;
	u32	poll_val;
	u32	poll_mask;
	u32	poll_delay;
	u32	poll_timeout;
};
#pragma pack(pop)

struct spi_bus_ops {
	unsigned long handle;
	u32 bus_width;

	int (*spi_xfer)(unsigned long handle,
			struct spi_bus_trans *spi_trans,
			u32 *status);

	int (*spi_xfer_async)(unsigned long handle,
			struct spi_bus_trans *spi_trans);

	int (*spi_sync)(unsigned long handle);
};

#endif /* ___SPI__BUS_H__ */
