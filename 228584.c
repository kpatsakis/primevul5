static void cil_reset_iomemcon(struct cil_iomemcon *iomemcon)
{
	if (iomemcon->context_str == NULL) {
		cil_reset_context(iomemcon->context);
	}
}