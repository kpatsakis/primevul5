static void __init mds_select_mitigation(void)
{
	if (!boot_cpu_has_bug(X86_BUG_MDS) || cpu_mitigations_off()) {
		mds_mitigation = MDS_MITIGATION_OFF;
		return;
	}

	if (mds_mitigation == MDS_MITIGATION_FULL) {
		if (!boot_cpu_has(X86_FEATURE_MD_CLEAR))
			mds_mitigation = MDS_MITIGATION_VMWERV;

		static_branch_enable(&mds_user_clear);

		if (!boot_cpu_has(X86_BUG_MSBDS_ONLY) &&
		    (mds_nosmt || cpu_mitigations_auto_nosmt()))
			cpu_smt_disable(false);
	}
}