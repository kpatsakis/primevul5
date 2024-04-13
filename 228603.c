static void cil_reset_devicetreecon(struct cil_devicetreecon *devicetreecon)
{
	if (devicetreecon->context_str == NULL) {
		cil_reset_context(devicetreecon->context);
	}
}