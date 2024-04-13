static inline struct eppoll_entry *ep_pwq_from_wait(wait_queue_entry_t *p)
{
	return container_of(p, struct eppoll_entry, wait);
}