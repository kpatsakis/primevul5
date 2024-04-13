static int pio_in_emulated(struct x86_emulate_ctxt *ctxt,
			   unsigned int size, unsigned short port,
			   void *dest)
{
	struct read_cache *rc = &ctxt->io_read;

	if (rc->pos == rc->end) { /* refill pio read ahead */
		unsigned int in_page, n;
		unsigned int count = ctxt->rep_prefix ?
			address_mask(ctxt, ctxt->regs[VCPU_REGS_RCX]) : 1;
		in_page = (ctxt->eflags & EFLG_DF) ?
			offset_in_page(ctxt->regs[VCPU_REGS_RDI]) :
			PAGE_SIZE - offset_in_page(ctxt->regs[VCPU_REGS_RDI]);
		n = min(min(in_page, (unsigned int)sizeof(rc->data)) / size,
			count);
		if (n == 0)
			n = 1;
		rc->pos = rc->end = 0;
		if (!ctxt->ops->pio_in_emulated(ctxt, size, port, rc->data, n))
			return 0;
		rc->end = n * size;
	}

	memcpy(dest, rc->data + rc->pos, size);
	rc->pos += size;
	return 1;
}