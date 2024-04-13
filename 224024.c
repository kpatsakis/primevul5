static void ebb_event_add(struct perf_event *event)
{
	if (!is_ebb_event(event) || current->thread.used_ebb)
		return;

	/*
	 * IFF this is the first time we've added an EBB event, set
	 * PMXE in the user MMCR0 so we can detect when it's cleared by
	 * userspace. We need this so that we can context switch while
	 * userspace is in the EBB handler (where PMXE is 0).
	 */
	current->thread.used_ebb = 1;
	current->thread.mmcr0 |= MMCR0_PMXE;
}