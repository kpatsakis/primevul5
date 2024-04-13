static void cil_reset_pcidevicecon(struct cil_pcidevicecon *pcidevicecon)
{
	if (pcidevicecon->context_str == NULL) {
		cil_reset_context(pcidevicecon->context);
	}
}