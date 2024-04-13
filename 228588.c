static void cil_reset_netifcon(struct cil_netifcon *netifcon)
{
	if (netifcon->if_context_str == NULL) {
		cil_reset_context(netifcon->if_context);
	}

	if (netifcon->packet_context_str == NULL) {
		cil_reset_context(netifcon->packet_context);
	}
}