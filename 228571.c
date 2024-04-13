static void cil_reset_nodecon(struct cil_nodecon *nodecon)
{
	if (nodecon->context_str == NULL) {
		cil_reset_context(nodecon->context);
	}
}