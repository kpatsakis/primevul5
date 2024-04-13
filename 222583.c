static int __init kvm_s390_init(void)
{
	int i;

	if (!sclp.has_sief2) {
		pr_info("SIE is not available\n");
		return -ENODEV;
	}

	if (nested && hpage) {
		pr_info("A KVM host that supports nesting cannot back its KVM guests with huge pages\n");
		return -EINVAL;
	}

	for (i = 0; i < 16; i++)
		kvm_s390_fac_base[i] |=
			S390_lowcore.stfle_fac_list[i] & nonhyp_mask(i);

	return kvm_init(NULL, sizeof(struct kvm_vcpu), 0, THIS_MODULE);
}