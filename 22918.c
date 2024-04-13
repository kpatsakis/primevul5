static inline int tipc_ehdr_size(struct tipc_ehdr *ehdr)
{
	return (ehdr->user != LINK_CONFIG) ? EHDR_SIZE : EHDR_CFG_SIZE;
}