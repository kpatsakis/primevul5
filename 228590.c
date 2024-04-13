static void cil_reset_portcon(struct cil_portcon *portcon)
{
	if (portcon->context_str == NULL) {
		cil_reset_context(portcon->context);
	}
}