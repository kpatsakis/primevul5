static void cil_reset_pirqcon(struct cil_pirqcon *pirqcon)
{
	if (pirqcon->context_str == NULL) {
		cil_reset_context(pirqcon->context);
	}
}