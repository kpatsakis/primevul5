static void kvm_s390_cpu_feat_init(void)
{
	int i;

	for (i = 0; i < 256; ++i) {
		if (plo_test_bit(i))
			kvm_s390_available_subfunc.plo[i >> 3] |= 0x80 >> (i & 7);
	}

	if (test_facility(28)) /* TOD-clock steering */
		ptff(kvm_s390_available_subfunc.ptff,
		     sizeof(kvm_s390_available_subfunc.ptff),
		     PTFF_QAF);

	if (test_facility(17)) { /* MSA */
		__cpacf_query(CPACF_KMAC, (cpacf_mask_t *)
			      kvm_s390_available_subfunc.kmac);
		__cpacf_query(CPACF_KMC, (cpacf_mask_t *)
			      kvm_s390_available_subfunc.kmc);
		__cpacf_query(CPACF_KM, (cpacf_mask_t *)
			      kvm_s390_available_subfunc.km);
		__cpacf_query(CPACF_KIMD, (cpacf_mask_t *)
			      kvm_s390_available_subfunc.kimd);
		__cpacf_query(CPACF_KLMD, (cpacf_mask_t *)
			      kvm_s390_available_subfunc.klmd);
	}
	if (test_facility(76)) /* MSA3 */
		__cpacf_query(CPACF_PCKMO, (cpacf_mask_t *)
			      kvm_s390_available_subfunc.pckmo);
	if (test_facility(77)) { /* MSA4 */
		__cpacf_query(CPACF_KMCTR, (cpacf_mask_t *)
			      kvm_s390_available_subfunc.kmctr);
		__cpacf_query(CPACF_KMF, (cpacf_mask_t *)
			      kvm_s390_available_subfunc.kmf);
		__cpacf_query(CPACF_KMO, (cpacf_mask_t *)
			      kvm_s390_available_subfunc.kmo);
		__cpacf_query(CPACF_PCC, (cpacf_mask_t *)
			      kvm_s390_available_subfunc.pcc);
	}
	if (test_facility(57)) /* MSA5 */
		__cpacf_query(CPACF_PRNO, (cpacf_mask_t *)
			      kvm_s390_available_subfunc.ppno);

	if (test_facility(146)) /* MSA8 */
		__cpacf_query(CPACF_KMA, (cpacf_mask_t *)
			      kvm_s390_available_subfunc.kma);

	if (test_facility(155)) /* MSA9 */
		__cpacf_query(CPACF_KDSA, (cpacf_mask_t *)
			      kvm_s390_available_subfunc.kdsa);

	if (test_facility(150)) /* SORTL */
		__insn32_query(INSN_SORTL, kvm_s390_available_subfunc.sortl);

	if (test_facility(151)) /* DFLTCC */
		__insn32_query(INSN_DFLTCC, kvm_s390_available_subfunc.dfltcc);

	if (MACHINE_HAS_ESOP)
		allow_cpu_feat(KVM_S390_VM_CPU_FEAT_ESOP);
	/*
	 * We need SIE support, ESOP (PROT_READ protection for gmap_shadow),
	 * 64bit SCAO (SCA passthrough) and IDTE (for gmap_shadow unshadowing).
	 */
	if (!sclp.has_sief2 || !MACHINE_HAS_ESOP || !sclp.has_64bscao ||
	    !test_facility(3) || !nested)
		return;
	allow_cpu_feat(KVM_S390_VM_CPU_FEAT_SIEF2);
	if (sclp.has_64bscao)
		allow_cpu_feat(KVM_S390_VM_CPU_FEAT_64BSCAO);
	if (sclp.has_siif)
		allow_cpu_feat(KVM_S390_VM_CPU_FEAT_SIIF);
	if (sclp.has_gpere)
		allow_cpu_feat(KVM_S390_VM_CPU_FEAT_GPERE);
	if (sclp.has_gsls)
		allow_cpu_feat(KVM_S390_VM_CPU_FEAT_GSLS);
	if (sclp.has_ib)
		allow_cpu_feat(KVM_S390_VM_CPU_FEAT_IB);
	if (sclp.has_cei)
		allow_cpu_feat(KVM_S390_VM_CPU_FEAT_CEI);
	if (sclp.has_ibs)
		allow_cpu_feat(KVM_S390_VM_CPU_FEAT_IBS);
	if (sclp.has_kss)
		allow_cpu_feat(KVM_S390_VM_CPU_FEAT_KSS);
	/*
	 * KVM_S390_VM_CPU_FEAT_SKEY: Wrong shadow of PTE.I bits will make
	 * all skey handling functions read/set the skey from the PGSTE
	 * instead of the real storage key.
	 *
	 * KVM_S390_VM_CPU_FEAT_CMMA: Wrong shadow of PTE.I bits will make
	 * pages being detected as preserved although they are resident.
	 *
	 * KVM_S390_VM_CPU_FEAT_PFMFI: Wrong shadow of PTE.I bits will
	 * have the same effect as for KVM_S390_VM_CPU_FEAT_SKEY.
	 *
	 * For KVM_S390_VM_CPU_FEAT_SKEY, KVM_S390_VM_CPU_FEAT_CMMA and
	 * KVM_S390_VM_CPU_FEAT_PFMFI, all PTE.I and PGSTE bits have to be
	 * correctly shadowed. We can do that for the PGSTE but not for PTE.I.
	 *
	 * KVM_S390_VM_CPU_FEAT_SIGPIF: Wrong SCB addresses in the SCA. We
	 * cannot easily shadow the SCA because of the ipte lock.
	 */
}