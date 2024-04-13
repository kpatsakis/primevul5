static inline void xt_write_recseq_end(unsigned int addend)
{
	/* this is kind of a write_seqcount_end(), but addend is 0 or 1 */
	smp_wmb();
	__this_cpu_add(xt_recseq.sequence, addend);
}