static void io_refs_resurrect(struct percpu_ref *ref, struct completion *compl)
{
	bool got = percpu_ref_tryget(ref);

	/* already at zero, wait for ->release() */
	if (!got)
		wait_for_completion(compl);
	percpu_ref_resurrect(ref);
	if (got)
		percpu_ref_put(ref);
}