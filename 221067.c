static struct epitem *ep_find_tfd(struct eventpoll *ep, int tfd, unsigned long toff)
{
	struct rb_node *rbp;
	struct epitem *epi;

	for (rbp = rb_first_cached(&ep->rbr); rbp; rbp = rb_next(rbp)) {
		epi = rb_entry(rbp, struct epitem, rbn);
		if (epi->ffd.fd == tfd) {
			if (toff == 0)
				return epi;
			else
				toff--;
		}
		cond_resched();
	}

	return NULL;
}