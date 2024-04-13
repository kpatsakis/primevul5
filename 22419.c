static int read_emulated(struct x86_emulate_ctxt *ctxt,
			 unsigned long addr, void *dest, unsigned size)
{
	int rc;
	struct read_cache *mc = &ctxt->mem_read;

	while (size) {
		int n = min(size, 8u);
		size -= n;
		if (mc->pos < mc->end)
			goto read_cached;

		rc = ctxt->ops->read_emulated(ctxt, addr, mc->data + mc->end, n,
					      &ctxt->exception);
		if (rc != X86EMUL_CONTINUE)
			return rc;
		mc->end += n;

	read_cached:
		memcpy(dest, mc->data + mc->pos, n);
		mc->pos += n;
		dest += n;
		addr += n;
	}
	return X86EMUL_CONTINUE;
}