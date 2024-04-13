static int __linearize(struct x86_emulate_ctxt *ctxt,
		     struct segmented_address addr,
		     unsigned size, bool write, bool fetch,
		     ulong *linear)
{
	struct desc_struct desc;
	bool usable;
	ulong la;
	u32 lim;
	u16 sel;
	unsigned cpl, rpl;

	la = seg_base(ctxt, addr.seg) + addr.ea;
	switch (ctxt->mode) {
	case X86EMUL_MODE_REAL:
		break;
	case X86EMUL_MODE_PROT64:
		if (((signed long)la << 16) >> 16 != la)
			return emulate_gp(ctxt, 0);
		break;
	default:
		usable = ctxt->ops->get_segment(ctxt, &sel, &desc, NULL,
						addr.seg);
		if (!usable)
			goto bad;
		/* code segment or read-only data segment */
		if (((desc.type & 8) || !(desc.type & 2)) && write)
			goto bad;
		/* unreadable code segment */
		if (!fetch && (desc.type & 8) && !(desc.type & 2))
			goto bad;
		lim = desc_limit_scaled(&desc);
		if ((desc.type & 8) || !(desc.type & 4)) {
			/* expand-up segment */
			if (addr.ea > lim || (u32)(addr.ea + size - 1) > lim)
				goto bad;
		} else {
			/* exapand-down segment */
			if (addr.ea <= lim || (u32)(addr.ea + size - 1) <= lim)
				goto bad;
			lim = desc.d ? 0xffffffff : 0xffff;
			if (addr.ea > lim || (u32)(addr.ea + size - 1) > lim)
				goto bad;
		}
		cpl = ctxt->ops->cpl(ctxt);
		rpl = sel & 3;
		cpl = max(cpl, rpl);
		if (!(desc.type & 8)) {
			/* data segment */
			if (cpl > desc.dpl)
				goto bad;
		} else if ((desc.type & 8) && !(desc.type & 4)) {
			/* nonconforming code segment */
			if (cpl != desc.dpl)
				goto bad;
		} else if ((desc.type & 8) && (desc.type & 4)) {
			/* conforming code segment */
			if (cpl < desc.dpl)
				goto bad;
		}
		break;
	}
	if (fetch ? ctxt->mode != X86EMUL_MODE_PROT64 : ctxt->ad_bytes != 8)
		la &= (u32)-1;
	*linear = la;
	return X86EMUL_CONTINUE;
bad:
	if (addr.seg == VCPU_SREG_SS)
		return emulate_ss(ctxt, addr.seg);
	else
		return emulate_gp(ctxt, addr.seg);
}