static bool nested_vmcb_check_controls(struct vmcb_control_area *control)
{
	if ((vmcb_is_intercept(control, INTERCEPT_VMRUN)) == 0)
		return false;

	if (control->asid == 0)
		return false;

	if ((control->nested_ctl & SVM_NESTED_CTL_NP_ENABLE) &&
	    !npt_enabled)
		return false;

	return true;
}