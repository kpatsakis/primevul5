static inline int ep_is_linked(struct epitem *epi)
{
	return !list_empty(&epi->rdllink);
}