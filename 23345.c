static irqreturn_t xennet_interrupt(int irq, void *dev_id)
{
	unsigned int eoiflag = XEN_EOI_FLAG_SPURIOUS;

	if (xennet_handle_tx(dev_id, &eoiflag) &&
	    xennet_handle_rx(dev_id, &eoiflag))
		xen_irq_lateeoi(irq, eoiflag);

	return IRQ_HANDLED;
}