
#include <linux/types.h>

#define LOMBO_UART_NUM  (4)

/* register offset define */
#define LOMBO_UART_DLR        (0x04)  /* divisor latch register */
#define LOMBO_UART_LPDLR      (0x08)  /* low power divisor latch register */
#define LOMBO_UART_THR        (0x0C)  /* transmit holding register */
#define LOMBO_UART_RBR        (0x10)  /* receiver buffer register */
#define LOMBO_UART_LCR        (0x14)  /* line control register */
#define LOMBO_UART_MCR        (0x18)  /* modem control register */
#define LOMBO_UART_FCR        (0x1C)  /* FIFO control register */
#define LOMBO_UART_LSR        (0x20)  /* line status register */
#define LOMBO_UART_MSR        (0x24)  /* modem status register */
#define LOMBO_UART_FSR        (0x28)  /* fifo status register */
#define LOMBO_UART_INT_EN     (0x2C)  /* interrupt enable register */
#define LOMBO_UART_INT_PD     (0x30)  /* interrupt pending register */
#define LOMBO_UART_INT_CLR    (0x34)  /* interrupt clear register */
#define LOMBO_UART_RS485TCR   (0x38)  /* rs485 transfer control register */
#define LOMBO_UART_RS485DET   (0x3C)  /* rs485 output enable timing register */
#define LOMBO_UART_RS485TAT   (0x40)  /* rs485 turn-around timing register */
#define LOMBO_UART_RAR        (0x44)  /* receive address register */
#define LOMBO_UART_TAR        (0x48)  /* transmit address register */
#define LOMBO_UART_TFR        (0x4C)  /* transmit fifo read */
#define LOMBO_UART_RFW        (0x50)  /* receive fifo write */

/* Interrupt Enable Register */
#define LOMBO_UART_RLS_ADDR_REC_E  (BIT(9))
#define LOMBO_UART_RLS_BI_E        (BIT(8))
#define LOMBO_UART_RLS_FE_E        (BIT(7))
#define LOMBO_UART_RLS_PE_E        (BIT(6))
#define LOMBO_UART_RLS_OV_E        (BIT(5))
#define LOMBO_UART_RX_DATA_AVL_E   (BIT(4))
#define LOMBO_UART_TX_EMPTY_TRG_E  (BIT(1))

/* Interrupt Pending Register */
#define LOMBO_UART_RLS_BI          (BIT(8))
#define LOMBO_UART_RLS_FE          (BIT(7))
#define LOMBO_UART_RLS_PE          (BIT(6))
#define LOMBO_UART_RLS_OV          (BIT(5))
#define LOMBO_UART_RX_DATA_AVL     (BIT(4))

/* Interrupt Clear Register */
#define LOMBO_UART_RLS_BI_CLR      (BIT(8))
#define LOMBO_UART_RLS_FE_CLR      (BIT(7))
#define LOMBO_UART_RLS_PE_CLR      (BIT(6))
#define LOMBO_UART_RLS_OV_CLR      (BIT(5))
#define LOMBO_UART_RX_DATA_AVL_CLR (BIT(4))

/* Line Status Rigster */
#define LOMBO_UART_LSR_TXETT       (BIT(5))
#define LOMBO_UART_LSR_BI          (BIT(4))
#define LOMBO_UART_LSR_FE          (BIT(3))
#define LOMBO_UART_LSR_PE          (BIT(2))
#define LOMBO_UART_LSR_OE          (BIT(1))
#define LOMBO_UART_LSR_DR          (BIT(0))
#define LOMBO_UART_LSR_BRK_ERROR_BITS 0x1E /* BI, FE, PE, OE bits */

enum uart_rx_trg {
	RX_TRG_1CH,  /* 1 character in the FIFO */
	RX_TRG_4CH,  /* 4 character in the FIFO */
	RX_TRG_8CH,  /* 8 character in the FIFO */
	RX_TRG_1_4,  /* FIFO 1/4 full */
	RX_TRG_1_2,  /* FIFO 1/2 full */
	RX_TRG_FULL  /* FIFO 2 less than full */
};

enum uart_tx_trg {
	TX_TRG_EMP,  /* FIFO empty */
	TX_TRG_2CH,  /* 2 character in the FIFO */
	TX_TRG_1_4,  /* FIFO 1/4 full */
	TX_TRG_1_2   /* FIFO 1/2 full */
};

enum uart_rs485_mode {
	FULL_DP_MODE, /* Full Duplex Mode */
	SW_HALF_DP_MODE, /* Software-Controlled Half Duplex */
	HW_HALF_DP_MODE, /* Hardware-Controlled Harf Duple */
};

extern void csp_uart_baud_rate_config(void *base, int clk_in, int baud);
extern void csp_uart_putchar(void *base, int c);
extern u32  csp_uart_getchar(void *base);
extern void csp_uart_dls_config(void *base, int bits);
extern void csp_uart_stop_config(void *base, int stop);
extern void csp_uart_parity_enable(void *base, int parity);
extern void csp_uart_set_parity_odd(void *base, int odd);
extern void csp_uart_reset(void *base);
extern void csp_uart_set_break_ctrl(void *base, u32 bc);
extern u32  csp_uart_is_auto_flow(void *base);
extern void csp_uart_set_auto_flow(void *base, int afce);
extern void csp_uart_set_rts(void *base);
extern void csp_uart_set_loop_back(void *base);
extern void csp_uart_tx_fifo_trg_config(void *base, enum uart_tx_trg trg);
extern void csp_uart_rx_fifo_trg_config(void *base, enum uart_rx_trg trg);
extern void csp_uart_fifo_enable(void *base);
extern void csp_uart_tx_fifo_reset(void *base);
extern u32  csp_uart_get_lsr(void *base);
extern u32  csp_uart_tx_empty(void *base);
extern u32  csp_uart_get_msr(void *base);
extern u32  csp_uart_is_cts(void *base);
extern u32  csp_uart_is_dcts(void *base);
extern u32  csp_uart_tx_fifo_not_full(void *base);
extern u32  csp_uart_tx_fifo_empty(void *base);
extern u32  csp_uart_irq_save(void *base);
extern void csp_uart_irq_restore(void *base, u32 irq);
extern void csp_uart_irq_disable(void *base);
extern void csp_uart_tx_irq_config(void *base, u32 enable);
extern void csp_uart_rx_rls_irq_disable(void *base);
extern void csp_uart_irq_enable(void *base);
extern u32  csp_uart_is_tx_empty_trg(void *base);
extern u32  csp_uart_is_rx_rls_or_data_avl(void *base);
extern void csp_uart_clear_tx_empty_trg(void *base);
extern void csp_uart_clear_rx_rls_or_data_avl(void *base);
extern void csp_uart_rs485_enable(void *base, u32 enable);
extern u32  csp_uart_rs485_mode_config(void *base, enum uart_rs485_mode mode);
extern u32  csp_uart_rs485_de_enable(void *base, u32 enable);
extern u32  csp_uart_rs485_re_enable(void *base, u32 enable);
extern void csp_uart_rs485_de_pol_enable(void *base, u32 enable);
extern void csp_uart_rs485_re_pol_enable(void *base, u32 enable);

