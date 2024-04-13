static inline void mark_all_dirty(struct vmcb *vmcb)
{
	vmcb->control.clean = 0;
}