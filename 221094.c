static inline bool chain_epi_lockless(struct epitem *epi)
{
	struct eventpoll *ep = epi->ep;

	/* Fast preliminary check */
	if (epi->next != EP_UNACTIVE_PTR)
		return false;

	/* Check that the same epi has not been just chained from another CPU */
	if (cmpxchg(&epi->next, EP_UNACTIVE_PTR, NULL) != EP_UNACTIVE_PTR)
		return false;

	/* Atomically exchange tail */
	epi->next = xchg(&ep->ovflist, epi);

	return true;
}