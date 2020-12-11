/*
 * lombo_uart.c - Driver of LomboTech UART Controller
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
#define DEBUG
#include <linux/clk.h>
#include <linux/clk-private.h>
#include <linux/console.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/of_irq.h>
#include <linux/platform_device.h>
#include <linux/serial_core.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>

#include <asm/setup.h>
#include "lombo_uart.h"

#define LOMBO_UART_BAUD_LV0	(1500000)
#define LOMBO_UART_BAUD_LV1	(3000000)

#define LOMBO_UART_CLK_LV0	(24000000)
#define LOMBO_UART_CLK_LV1	(50000000)
#define LOMBO_UART_CLK_LV2	(150000000)

static void lombo_uart_stop_tx(struct uart_port *port);
static struct lombo_uart_port lombo_uart_ports[LOMBO_UART_NUM];

static unsigned int __uart_handle_rx(struct lombo_uart_port *eup)
{
	unsigned int lsr = 0;
	unsigned int ch = 0;
	int max_count = 256;
	char flag;

	lsr = csp_uart_get_lsr(eup->uport.membase);

	do {
		if (likely(lsr & LOMBO_UART_LSR_DR))
			ch = csp_uart_getchar(eup->uport.membase);

		flag = TTY_NORMAL;
		eup->uport.icount.rx++;

		if (unlikely(lsr & LOMBO_UART_LSR_BRK_ERROR_BITS)) {
			/* For statistics only */
			if (lsr & LOMBO_UART_LSR_BI) {
				lsr &= ~(LOMBO_UART_LSR_PE | LOMBO_UART_LSR_FE);
				eup->uport.icount.brk++;
				if (uart_handle_break(&eup->uport))
					goto ignore_char;
			} else if (lsr & LOMBO_UART_LSR_PE) {
				eup->uport.icount.parity++;
			} else if (lsr & LOMBO_UART_LSR_FE) {
				eup->uport.icount.frame++;
			}

			if (lsr & LOMBO_UART_LSR_OE)
				eup->uport.icount.overrun++;

			/* mask off conditions which should be ignored */
			lsr &= eup->uport.read_status_mask;

			if (lsr & LOMBO_UART_LSR_BI)
				flag = TTY_BREAK;
			else if (lsr & LOMBO_UART_LSR_PE)
				flag = TTY_PARITY;
			else if (lsr & LOMBO_UART_LSR_FE)
				flag = TTY_FRAME;
		}

		if (uart_handle_sysrq_char(&eup->uport, ch))
			goto ignore_char;

		uart_insert_char(&eup->uport, lsr, LOMBO_UART_LSR_OE, ch, flag);
ignore_char:
		lsr = csp_uart_get_lsr(eup->uport.membase);
	} while ((lsr & (LOMBO_UART_LSR_DR | LOMBO_UART_LSR_BI))
		&& (max_count-- > 0));

	spin_unlock(&eup->uport.lock);
	tty_flip_buffer_push(&eup->uport.state->port);
	spin_lock(&eup->uport.lock);

	return lsr;
}

static void __uart_handle_tx(struct lombo_uart_port *eup)
{
	struct circ_buf *xmit = &eup->uport.state->xmit;
	int count;

	if (eup->uport.x_char) {
		csp_uart_putchar(eup->uport.membase, eup->uport.x_char);
		eup->uport.icount.tx++;
		eup->uport.x_char = 0;
		return;
	}

	if (uart_circ_empty(xmit) || uart_tx_stopped(&eup->uport)) {
		lombo_uart_stop_tx(&eup->uport);
		return;
	}

	count = eup->uport.fifosize / 2;

	do {
		csp_uart_putchar(eup->uport.membase, xmit->buf[xmit->tail]);
		xmit->tail = (xmit->tail + 1) & (UART_XMIT_SIZE - 1);
		eup->uport.icount.tx++;

		if (uart_circ_empty(xmit))
			break;
	} while (--count > 0);

	if (uart_circ_chars_pending(xmit) < WAKEUP_CHARS) {
		spin_unlock(&eup->uport.lock);
		uart_write_wakeup(&eup->uport);
		spin_lock(&eup->uport.lock);
	}

	if (uart_circ_empty(xmit))
		lombo_uart_stop_tx(&eup->uport);
}

static void __uart_modem_status(struct uart_port *port)
{
	if (csp_uart_is_dcts(port->membase) && port->state != NULL) {
		if (!csp_uart_is_auto_flow(port->membase))
			uart_handle_cts_change(port,
						csp_uart_is_cts(port->membase));

		wake_up_interruptible(&port->state->port.delta_msr_wait);
	}
}

static irqreturn_t lombo_uart_irq_handler(int irq, void *dev_id)
{
	struct uart_port *port = dev_id;
	struct lombo_uart_port *eup = to_lombo_uport(port);
	unsigned long flags;

	spin_lock_irqsave(&port->lock, flags);

	/* receive line status interrupt:BI/FE/PE/OV, or rx data available */
	if (csp_uart_is_rx_rls_or_data_avl(port->membase)) {
		__uart_handle_rx(eup);
		csp_uart_clear_rx_rls_or_data_avl(port->membase);
	}

	if (eup->io_num == 4)
		__uart_modem_status(port);

	/* tx fifo empty */
	if (csp_uart_is_tx_empty_trg(port->membase)) {
		__uart_handle_tx(eup);
		csp_uart_clear_tx_empty_trg(port->membase);
	}

	spin_unlock_irqrestore(&port->lock, flags);

	return IRQ_HANDLED;
}

static unsigned int lombo_uart_tx_empty(struct uart_port *port)
{
	unsigned long flags = 0;
	unsigned int ret = 0;

	spin_lock_irqsave(&port->lock, flags);
	ret = csp_uart_tx_fifo_empty(port->membase) ? TIOCSER_TEMT : 0;
	spin_unlock_irqrestore(&port->lock, flags);

	return ret;
}

static void lombo_uart_set_mctrl(struct uart_port *port, unsigned int mctrl)
{
	if (mctrl & TIOCM_RTS)
		csp_uart_set_rts(port->membase);

	if (mctrl & TIOCM_LOOP)
		csp_uart_set_loop_back(port->membase);
}

static unsigned int lombo_uart_get_mctrl(struct uart_port *port)
{
	return csp_uart_is_cts(port->membase) ? TIOCM_CTS : 0;
}

static void lombo_uart_stop_tx(struct uart_port *port)
{
	csp_uart_tx_irq_config(port->membase, 0);
}

static void lombo_uart_start_tx(struct uart_port *port)
{
	csp_uart_tx_irq_config(port->membase, 1);
}

static void lombo_uart_stop_rx(struct uart_port *port)
{
	struct lombo_uart_port *eup = to_lombo_uport(port);

	eup->uport.read_status_mask &= ~LOMBO_UART_LSR_DR;
	csp_uart_rx_rls_irq_disable(port->membase);
}

static void lombo_uart_break_ctl(struct uart_port *port, int break_state)
{
	unsigned long flags;

	spin_lock_irqsave(&port->lock, flags);
	csp_uart_set_break_ctrl(port->membase, break_state);
	spin_unlock_irqrestore(&port->lock, flags);
}

static int lombo_uart_startup(struct uart_port *port)
{
	struct lombo_uart_port *eup = to_lombo_uport(port);
	int ret;

	ret = request_irq(port->irq, lombo_uart_irq_handler, 0,
				eup->name, port);
	if (unlikely(ret)) {
		PRT_ERR("uart%d: can not request irq %d\n",
				eup->id, port->irq);
		return ret;
	}

	/* reset uart */
	if (!lombo_is_console_port(port))
		csp_uart_reset(port->membase);

	return 0;
}

static void lombo_uart_shutdown(struct uart_port *port)
{
	free_irq(port->irq, port);
}

static void lombo_uart_flush_buffer(struct uart_port *port)
{
	csp_uart_tx_fifo_reset(port->membase);
}

static void lombo_uart_set_termios(struct uart_port *port,
		struct ktermios *termios, struct ktermios *old)
{
	struct lombo_uart_port *eup = to_lombo_uport(port);
	unsigned long flags;
	unsigned long uart_clock_in;
	unsigned int baud;
	unsigned int baud_check;
	int bits = 8, parity = 0;
	int rate = 0;

	switch (termios->c_cflag & CSIZE) {
	case CS5:
		bits = 5;
		break;
	case CS6:
		bits = 6;
		break;
	case CS7:
		bits = 7;
		break;
	case CS8:
	default:
		break;
	}

	/* set uart data length */
	csp_uart_dls_config(port->membase, bits);

	if (termios->c_cflag & PARENB) {
		parity = 1;

		/* set uart parity, odd or even */
		csp_uart_set_parity_odd(port->membase,
				termios->c_cflag & PARODD);
	}

	/* set uart parity enable or not */
	csp_uart_parity_enable(port->membase, parity);

	/* set uart stop bits */
	csp_uart_stop_config(port->membase, termios->c_cflag & CSTOPB);

	baud = tty_termios_baud_rate(termios);

	/* disable uart clk */
	clk_disable_unprepare(eup->uart_clk);

	/* put uart parent clk */
	clk_put(eup->parent);

	if (baud <= LOMBO_UART_BAUD_LV0) {
		eup->parent = of_clk_get(port->dev->of_node, 3);
		rate = LOMBO_UART_CLK_LV0;
	} else if (baud <= LOMBO_UART_BAUD_LV1) {
		eup->parent = of_clk_get(port->dev->of_node, 4);
		rate = LOMBO_UART_CLK_LV1;
	} else {
		eup->parent = of_clk_get(port->dev->of_node, 4);
		rate = LOMBO_UART_CLK_LV2;
	}

	/* set uart parent clk */
	if (clk_set_parent(eup->uart_clk, eup->parent)) {
		PRT_ERR("uart%d: set parent clk failed\n", eup->id);
		return;
	}

	/* set uart clk rate */
	if (clk_set_rate(eup->uart_clk, rate)) {
		PRT_ERR("uart%d: set clk rate to %d failed\n", eup->id, rate);
		return;
	}

	/* enable uart clk */
	if (clk_prepare_enable(eup->uart_clk)) {
		PRT_ERR("uart%d: enable uart clock failed\n", eup->id);
		return;
	}

	port->uartclk = clk_get_rate(eup->uart_clk);
	baud_check = uart_get_baud_rate(port, termios, old,
		port->uartclk / 16 / 0xffff, port->uartclk / 16);

	if (baud != baud_check) {
		PRT_ERR("uart%d: baud-%u != baud_check-%u\n",
				eup->id, baud, baud_check);
		BUG();
	}

	uart_clock_in = clk_get_rate(eup->uart_clk);

	PRT_DBG("uart%d: clk_parent=%lu clk_in=%lu baud=%u\n", eup->id,
		clk_get_rate(eup->parent), uart_clock_in, baud);

	/* set buadrate */
	csp_uart_baud_rate_config(port->membase, uart_clock_in, baud);

	spin_lock_irqsave(&port->lock, flags);

	/* update the per-port timeout. */
	uart_update_timeout(port, termios->c_cflag, baud);

	port->read_status_mask = LOMBO_UART_LSR_OE
				| LOMBO_UART_LSR_TXETT
				| LOMBO_UART_LSR_DR;

	if (termios->c_iflag & INPCK)
		port->read_status_mask |= LOMBO_UART_LSR_FE | LOMBO_UART_LSR_PE;

	if (termios->c_iflag & (BRKINT | PARMRK))
		port->read_status_mask |= LOMBO_UART_LSR_BI;

	/* characteres to ignore */
	port->ignore_status_mask = 0;

	if (termios->c_iflag & IGNPAR)
		port->ignore_status_mask |= LOMBO_UART_LSR_PE
					| LOMBO_UART_LSR_FE;

	if (termios->c_iflag & IGNBRK) {
		port->ignore_status_mask |= LOMBO_UART_LSR_BI;
		/*
		 * if we're ignoring parity and break indicators,
		 * ignore overruns too (for real raw support).
		 */
		if (termios->c_iflag & IGNPAR)
			port->ignore_status_mask |= LOMBO_UART_LSR_OE;
	}

	/* ignore all characters if CREAD is not set */
	if ((termios->c_cflag & CREAD) == 0)
		port->ignore_status_mask |= LOMBO_UART_LSR_DR;

	/* flow control config */
	csp_uart_set_auto_flow(port->membase, termios->c_cflag & CRTSCTS);

	/* enable uart irq */
	csp_uart_irq_enable(port->membase);

	/* set uart tx fifo trigger level */
	csp_uart_tx_fifo_trg_config(port->membase, TX_TRG_1_2);

	/* set uart rx fifo trigger level */
	csp_uart_rx_fifo_trg_config(port->membase, RX_TRG_1CH);

	/* enable uart fifo */
	csp_uart_fifo_enable(port->membase);

	port->ops->set_mctrl(port, port->mctrl);

	/* must save the current config for the resume of console */
	if (lombo_is_console_port(port))
		port->cons->cflag = termios->c_cflag;

	spin_unlock_irqrestore(&port->lock, flags);

	/* don't rewrite B0 */
	if (tty_termios_baud_rate(termios))
		tty_termios_encode_baud_rate(termios, baud, baud);
}

static void lombo_uart_pm(struct uart_port *port,
		unsigned int state, unsigned int oldstate)
{
	struct lombo_uart_port *eup = to_lombo_uport(port);
#ifndef CONFIG_LOMBO_MIN_UART_SUPPORT
	int ret;
#endif

	switch (state) {
	case UART_PM_STATE_ON: /* power up */
#ifndef CONFIG_LOMBO_MIN_UART_SUPPORT
		if (eup->uart_clk->enable_count > 0)
			break;

		ret = clk_prepare_enable(eup->uart_clk);
		if (ret)
			PRT_ERR("uart%d: clk enable failed\n", eup->id);
#endif

		break;
	case UART_PM_STATE_OFF: /* power down */
#ifndef CONFIG_LOMBO_MIN_UART_SUPPORT
		if (eup->uart_clk->enable_count == 0)
			break;

		clk_disable_unprepare(eup->uart_clk);
#endif
		break;
	default:
		PRT_ERR("uart%d: unknown PM state %d\n", eup->id, state);
	}
}

static const char *lombo_uart_type(struct uart_port *port)
{
	return "LOMBO";
}

#ifndef CONFIG_LOMBO_MIN_UART_SUPPORT
static int __uart_select_gpio_state(struct pinctrl *pctrl, char *name, u32 no)
{
	int ret = 0;
	struct pinctrl_state *pctrl_state = NULL;

	pctrl_state = pinctrl_lookup_state(pctrl, name);
	if (IS_ERR(pctrl_state)) {
		PRT_ERR("uart%d: pinctrl_lookup_state(%s) failed! return %p\n",
				no, name, pctrl_state);
		return -1;
	}

	ret = pinctrl_select_state(pctrl, pctrl_state);
	if (ret < 0) {
		PRT_ERR("uart%d: pinctrl_select_state(%s) failed! return %d\n",
				no, name, ret);
	}

	return ret;
}

static int __uart_request_gpio(struct lombo_uart_port *eup)
{
	eup->pctrl = devm_pinctrl_get(eup->uport.dev);

	if (IS_ERR_OR_NULL(eup->pctrl)) {
		PRT_ERR("uart%d: devm_pinctrl_get() failed! return %ld\n",
				eup->id, PTR_ERR(eup->pctrl));
		return -1;
	}

	return __uart_select_gpio_state(eup->pctrl,
				PINCTRL_STATE_DEFAULT, eup->id);
}

static void __uart_release_gpio(struct lombo_uart_port *eup)
{
	devm_pinctrl_put(eup->pctrl);
	eup->pctrl = NULL;
}
#endif

static void lombo_uart_release_port(struct uart_port *port)
{
	struct platform_device *pdev = to_platform_device(port->dev);
	struct lombo_uart_port *eup = to_lombo_uport(port);
	struct resource *mem_res;

	mem_res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (mem_res == NULL) {
		PRT_ERR("uart%d, get MEM resource failed\n", eup->id);
		return;
	}

	/* release memory resource */
	release_mem_region(mem_res->start, resource_size(mem_res));
	iounmap(port->membase);
	port->membase = NULL;

#ifndef CONFIG_LOMBO_MIN_UART_SUPPORT
	/* release pin resource */
	__uart_release_gpio(eup);
#endif
}

static int lombo_uart_request_port(struct uart_port *port)
{
	struct platform_device *pdev = to_platform_device(port->dev);
	struct lombo_uart_port *eup = to_lombo_uport(port);
	struct resource *mem_res;
#ifndef CONFIG_LOMBO_MIN_UART_SUPPORT
	int ret;
#endif

	mem_res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (mem_res == NULL) {
		PRT_ERR("uart%d, get MEM resource failed\n", eup->id);
		return -ENXIO;
	}

	/* request memory resource */
	if (!request_mem_region(mem_res->start, resource_size(mem_res),
		LOMBO_UART_DEV_NAME)) {
		PRT_ERR("uart%d, request mem region failed\n", eup->id);
		return -EBUSY;
	}

	/* ioremap port->membase */
	port->membase = ioremap(mem_res->start, resource_size(mem_res));
	if (!port->membase) {
		PRT_ERR("uart%d, ioremap failed\n", eup->id);
		release_mem_region(mem_res->start, resource_size(mem_res));
		return -EBUSY;
	}

#ifndef CONFIG_LOMBO_MIN_UART_SUPPORT
	/* request pin resource */
	ret = __uart_request_gpio(eup);
	if (ret < 0) {
		PRT_ERR("__uart_request_gpio failed, ret=%d\n", ret);
		iounmap(port->membase);
		release_mem_region(mem_res->start, resource_size(mem_res));
		return ret;
	}
#endif

	return 0;
}

static void lombo_uart_config_port(struct uart_port *port, int flags)
{
	int ret;

	if (flags & UART_CONFIG_TYPE) {
		port->type = PORT_LOMBO;
		ret = lombo_uart_request_port(port);
		if (ret)
			return;
	}
}

static int lombo_uart_verify_port(struct uart_port *port,
		struct serial_struct *ser)
{
	if (unlikely((ser->type != PORT_UNKNOWN) && (ser->type != PORT_LOMBO)))
		return -EINVAL;

	if (unlikely(port->irq != ser->irq))
		return -EINVAL;

	return 0;
}

static struct uart_ops lombo_uart_ops = {
	.tx_empty     = lombo_uart_tx_empty,
	.set_mctrl    = lombo_uart_set_mctrl,
	.get_mctrl    = lombo_uart_get_mctrl,
	.stop_tx      = lombo_uart_stop_tx,
	.start_tx     = lombo_uart_start_tx,
	.stop_rx      = lombo_uart_stop_rx,
	.break_ctl    = lombo_uart_break_ctl,
	.startup      = lombo_uart_startup,
	.shutdown     = lombo_uart_shutdown,
	.flush_buffer = lombo_uart_flush_buffer,
	.set_termios  = lombo_uart_set_termios,
	.pm           = lombo_uart_pm,
	.type         = lombo_uart_type,
	.release_port = lombo_uart_release_port,
	.request_port = lombo_uart_request_port,
	.config_port  = lombo_uart_config_port,
	.verify_port  = lombo_uart_verify_port,
};

#ifdef CONFIG_SERIAL_LOMBO_CONSOLE
static void csp_uart_wait_xfer_done(struct uart_port *port)
{
	unsigned int timeout = 1000000;

	while (timeout--) {
		if (csp_uart_tx_empty(port->membase))
			break;

		udelay(1);
	}

	WARN_ON(timeout == 0);
}

static void lombo_console_putchar(struct uart_port *port, int c)
{
	unsigned int timeout = 1000000;
	struct lombo_uart_port *eup = to_lombo_uport(port);

	/* wait tx fifo not full */
	while (!csp_uart_tx_fifo_not_full(port->membase))
		;

	/* CTS is supported by the 4 line UART */
	if (eup->io_num == 4) {
		/* wait up to 1000ms for flow control if necessary */
		if (port->flags & UPF_CONS_FLOW) {
			while (timeout--) {
				if (csp_uart_is_cts(port->membase))
					break;

				udelay(1);
			}
		}
	}

	/* xfer one character */
	csp_uart_putchar(port->membase, c);
}

static void lombo_console_write(struct console *co, const char *s,
		unsigned int count)
{
	struct uart_port *port = NULL;
	unsigned long flags;
	unsigned int ier;
	int locked = 1;

	BUG_ON(co->index < 0 || co->index >= LOMBO_UART_NUM);

	port = &lombo_uart_ports[co->index].uport;

	local_irq_save(flags);

	if (port->sysrq)
		locked = 0;
	else if (oops_in_progress)
		locked = spin_trylock(&port->lock);
	else
		spin_lock(&port->lock);

	/* save uart irq enable status */
	ier = csp_uart_irq_save(port->membase);

	/* disable all uart irq */
	csp_uart_irq_disable(port->membase);

	uart_console_write(port, s, count, lombo_console_putchar);

	/* wait uart xfer done */
	csp_uart_wait_xfer_done(port);

	/* restore uart irq enable status */
	csp_uart_irq_restore(port->membase, ier);

	if (locked)
		spin_unlock(&port->lock);

	local_irq_restore(flags);
}

static int __init lombo_console_setup(struct console *co, char *options)
{
	struct uart_port *port = NULL;
	int baud   = 115200;
	int bits   = 8;
	int parity = 'n';
	int flow   = 'n';

	/* is this a valid port */
	if (unlikely(co->index >= LOMBO_UART_NUM || co->index < 0))
		co->index = 0;

	port = &lombo_uart_ports[co->index].uport;
	if (!port->membase)
		return -ENODEV;

	if (options)
		uart_parse_options(options, &baud, &parity, &bits, &flow);

	return uart_set_options(port, co, baud, parity, bits, flow);
}

static struct uart_driver lombo_uart_driver;
static struct console lombo_console = {
	.name   = "ttySLB",
	.write  = lombo_console_write,
	.device = uart_console_device,
	.setup  = lombo_console_setup,
	.flags  = CON_PRINTBUFFER | CON_ANYTIME,
	.index  = -1,
	.data   = &lombo_uart_driver,
};
#endif /* CONFIG_SERIAL_LOMBO_CONSOLE */

static struct uart_driver lombo_uart_driver = {
	.owner = THIS_MODULE,
	.driver_name = LOMBO_UART_DEV_NAME,
	.dev_name = "ttySLB",
	.nr = LOMBO_UART_NUM,
#ifdef CONFIG_SERIAL_LOMBO_CONSOLE
	.cons = &lombo_console,
#endif
};

static int lombo_uart_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	struct uart_port *port;
	struct lombo_uart_port *eup;
	struct resource *res;
	int err = 0;

	pdev->id = of_alias_get_id(np, "serial");
	if (pdev->id < 0) {
		PRT_ERR("get alias id failed\n");
		return -EINVAL;
	}

	port = &lombo_uart_ports[pdev->id].uport;
	eup = to_lombo_uport(port);
	eup->id = pdev->id;
	snprintf(eup->name, 16, LOMBO_UART_DEV_NAME"%d", pdev->id);
	pdev->dev.init_name = eup->name;

	port->dev = &pdev->dev;

	err = of_property_read_u32(np, "uart_type", &eup->io_num);
	if (err) {
		PRT_ERR("uart%d error to get type property\n", pdev->id);
		return -EINVAL;
	}

	/* get IO */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (res == NULL) {
		PRT_ERR("uart%d error to get MEM resource\n", pdev->id);
		return -EINVAL;
	}
	port->mapbase = res->start;

	/* get IRQ */
	port->irq = platform_get_irq(pdev, 0);
	if (port->irq < 0) {
		PRT_ERR("uart%d error to get irq\n", pdev->id);
		return -EINVAL;
	}

#ifndef CONFIG_LOMBO_MIN_UART_SUPPORT
	/* get uart apb gate */
	eup->apb_gate = of_clk_get(np, 0);
	if (IS_ERR(eup->apb_gate)) {
		PRT_ERR("uart%d get apb gate clk failed\n", pdev->id);
		return -EINVAL;
	}

	/* enable uart apb gate */
	err = clk_prepare_enable(eup->apb_gate);
	if (err) {
		PRT_ERR("uart%d enable apb gate clk failed\n", pdev->id);
		return err;
	}

	/* get uart apb reset */
	eup->apb_reset = of_clk_get(np, 1);
	if (IS_ERR(eup->apb_reset)) {
		PRT_ERR("uart%d get apb reset clk failed\n", pdev->id);
		return -EINVAL;
	}

	/* enable uart apb reset */
	err = clk_prepare_enable(eup->apb_reset);
	if (err) {
		PRT_ERR("uart%d enable apb reset clk failed\n", pdev->id);
		return err;
	}

	/* get uart module clk */
	eup->uart_clk = of_clk_get(np, 2);
	if (IS_ERR(eup->uart_clk)) {
		PRT_ERR("uart%d get clk failed\n", pdev->id);
		return -EINVAL;
	}

	/* get uart parent clk */
	eup->parent = of_clk_get(np, 3);
	if (IS_ERR(eup->parent)) {
		PRT_ERR("uart%d get parent clk failed\n", pdev->id);
		return -EINVAL;
	}

	/* set uart parent clk */
	err = clk_set_parent(eup->uart_clk, eup->parent);
	if (err) {
		PRT_ERR("uart%d set parent clk failed\n", pdev->id);
		return err;
	}
#endif

#ifndef CONFIG_LOMBO_MIN_UART_SUPPORT
	port->uartclk = clk_get_rate(eup->uart_clk);
#else
	port->uartclk = 24000000;
#endif

	port->line     = pdev->id;
	port->iotype   = UPIO_MEM;
	port->type     = PORT_LOMBO;
	port->flags    = UPF_BOOT_AUTOCONF;
	port->fifosize = 64;
	port->ops      = &lombo_uart_ops;

	platform_set_drvdata(pdev, port);

	err = uart_add_one_port(&lombo_uart_driver, port);
	if (err) {
		PRT_ERR("uart%d add port failed\n", pdev->id);
		return -EINVAL;
	}

	PRT_INFO("uart%d: probe succeed, irq:%d, clk in:%u\n",
		port->line, port->irq, port->uartclk);

	return 0;
}

static int lombo_uart_remove(struct platform_device *pdev)
{
	struct uart_port *port = platform_get_drvdata(pdev);
	int err = 0;
#ifndef CONFIG_LOMBO_MIN_UART_SUPPORT
	struct lombo_uart_port *eup = to_lombo_uport(port);
#endif

	err = uart_remove_one_port(&lombo_uart_driver, port);
	if (err) {
		PRT_ERR("uart%d remove port failed\n", pdev->id);
		return -EINVAL;
	}

#ifndef CONFIG_LOMBO_MIN_UART_SUPPORT
	clk_disable_unprepare(eup->uart_clk);
	clk_put(eup->uart_clk);
#endif

	return 0;
}

static const struct of_device_id lombo_uart_match[] = {
	{ .compatible = "lombo,n7v0-uart", },
	{ .compatible = "lombo,n7v1-uart", },
	{},
};
MODULE_DEVICE_TABLE(of, lombo_uart_match);

#ifdef CONFIG_PM_SLEEP
static int lombo_uart_suspend(struct device *dev)
{
	struct uart_port *port = dev_get_drvdata(dev);
	return uart_suspend_port(&lombo_uart_driver, port);
}

static int lombo_uart_resume(struct device *dev)
{
	struct uart_port *port = dev_get_drvdata(dev);
	return uart_resume_port(&lombo_uart_driver, port);
}

static const struct dev_pm_ops lombo_uart_pm_ops = {
	.suspend = lombo_uart_suspend,
	.resume  = lombo_uart_resume,
};
#endif /* CONFIG_PM_SLEEP */

static struct platform_driver lombo_uart_platform_driver = {
	.probe  = lombo_uart_probe,
	.remove = lombo_uart_remove,
	.driver = {
		.name  = LOMBO_UART_DEV_NAME,
		.owner = THIS_MODULE,
		.of_match_table = lombo_uart_match,
#ifdef CONFIG_PM_SLEEP
		.pm    = &lombo_uart_pm_ops,
#endif
	},
};

static int __init lombo_uart_init(void)
{
	int err = 0;

	err = uart_register_driver(&lombo_uart_driver);
	if (err) {
		PRT_ERR("lombo uart driver register failed\n");
		return err;
	}

	return platform_driver_register(&lombo_uart_platform_driver);
}
module_init(lombo_uart_init);

static void __exit lombo_uart_exit(void)
{
#ifdef CONFIG_SERIAL_LOMBO_CONSOLE
	unregister_console(&lombo_console);
#endif
	platform_driver_unregister(&lombo_uart_platform_driver);
	uart_unregister_driver(&lombo_uart_driver);
}
module_exit(lombo_uart_exit);

MODULE_AUTHOR("lomboswer@lombotech.com>");
MODULE_DESCRIPTION("Driver for LomboTech UART controller");
MODULE_LICENSE("GPL");
