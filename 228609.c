static void cil_reset_ibpkeycon(struct cil_ibpkeycon *ibpkeycon)
{
	if (!ibpkeycon->context_str)
		cil_reset_context(ibpkeycon->context);
}