static bool is_event_blacklisted(u64 ev)
{
	int i;

	for (i=0; i < ppmu->n_blacklist_ev; i++) {
		if (ppmu->blacklist_ev[i] == ev)
			return true;
	}

	return false;
}