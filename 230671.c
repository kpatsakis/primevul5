static bool nested_vmcb_checks(struct vmcb *vmcb)
{
	if ((vmcb->control.intercept & (1ULL << INTERCEPT_VMRUN)) == 0)
		return false;

	if (vmcb->control.asid == 0)
		return false;

	if ((vmcb->control.nested_ctl & SVM_NESTED_CTL_NP_ENABLE) &&
	    !npt_enabled)
		return false;

	return true;
}