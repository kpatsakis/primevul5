static void power_pmu_bhrb_read(struct perf_event *event, struct cpu_hw_events *cpuhw)
{
	u64 val;
	u64 addr;
	int r_index, u_index, pred;

	r_index = 0;
	u_index = 0;
	while (r_index < ppmu->bhrb_nr) {
		/* Assembly read function */
		val = read_bhrb(r_index++);
		if (!val)
			/* Terminal marker: End of valid BHRB entries */
			break;
		else {
			addr = val & BHRB_EA;
			pred = val & BHRB_PREDICTION;

			if (!addr)
				/* invalid entry */
				continue;

			/*
			 * BHRB rolling buffer could very much contain the kernel
			 * addresses at this point. Check the privileges before
			 * exporting it to userspace (avoid exposure of regions
			 * where we could have speculative execution)
			 * Incase of ISA v3.1, BHRB will capture only user-space
			 * addresses, hence include a check before filtering code
			 */
			if (!(ppmu->flags & PPMU_ARCH_31) &&
			    is_kernel_addr(addr) && event->attr.exclude_kernel)
				continue;

			/* Branches are read most recent first (ie. mfbhrb 0 is
			 * the most recent branch).
			 * There are two types of valid entries:
			 * 1) a target entry which is the to address of a
			 *    computed goto like a blr,bctr,btar.  The next
			 *    entry read from the bhrb will be branch
			 *    corresponding to this target (ie. the actual
			 *    blr/bctr/btar instruction).
			 * 2) a from address which is an actual branch.  If a
			 *    target entry proceeds this, then this is the
			 *    matching branch for that target.  If this is not
			 *    following a target entry, then this is a branch
			 *    where the target is given as an immediate field
			 *    in the instruction (ie. an i or b form branch).
			 *    In this case we need to read the instruction from
			 *    memory to determine the target/to address.
			 */

			if (val & BHRB_TARGET) {
				/* Target branches use two entries
				 * (ie. computed gotos/XL form)
				 */
				cpuhw->bhrb_entries[u_index].to = addr;
				cpuhw->bhrb_entries[u_index].mispred = pred;
				cpuhw->bhrb_entries[u_index].predicted = ~pred;

				/* Get from address in next entry */
				val = read_bhrb(r_index++);
				addr = val & BHRB_EA;
				if (val & BHRB_TARGET) {
					/* Shouldn't have two targets in a
					   row.. Reset index and try again */
					r_index--;
					addr = 0;
				}
				cpuhw->bhrb_entries[u_index].from = addr;
			} else {
				/* Branches to immediate field 
				   (ie I or B form) */
				cpuhw->bhrb_entries[u_index].from = addr;
				cpuhw->bhrb_entries[u_index].to =
					power_pmu_bhrb_to(addr);
				cpuhw->bhrb_entries[u_index].mispred = pred;
				cpuhw->bhrb_entries[u_index].predicted = ~pred;
			}
			u_index++;

		}
	}
	cpuhw->bhrb_stack.nr = u_index;
	cpuhw->bhrb_stack.hw_idx = -1ULL;
	return;
}