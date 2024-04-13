static bool svm_has_emulated_msr(int index)
{
	switch (index) {
	case MSR_IA32_MCG_EXT_CTL:
	case MSR_IA32_VMX_BASIC ... MSR_IA32_VMX_VMFUNC:
		return false;
	default:
		break;
	}

	return true;
}