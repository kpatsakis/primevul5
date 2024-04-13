static irqreturn_t xennet_rx_interrupt(int irq, void *dev_id)
{
	unsigned int eoiflag = XEN_EOI_FLAG_SPURIOUS;

	if (likely(xennet_handle_rx(dev_id, &eoiflag)))
		xen_irq_lateeoi(irq, eoiflag);

	return IRQ_HANDLED;
}