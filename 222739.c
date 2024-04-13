int kvm_arch_init(void *opaque)
{
	int rc = -ENOMEM;

	kvm_s390_dbf = debug_register("kvm-trace", 32, 1, 7 * sizeof(long));
	if (!kvm_s390_dbf)
		return -ENOMEM;

	kvm_s390_dbf_uv = debug_register("kvm-uv", 32, 1, 7 * sizeof(long));
	if (!kvm_s390_dbf_uv)
		goto out;

	if (debug_register_view(kvm_s390_dbf, &debug_sprintf_view) ||
	    debug_register_view(kvm_s390_dbf_uv, &debug_sprintf_view))
		goto out;

	kvm_s390_cpu_feat_init();

	/* Register floating interrupt controller interface. */
	rc = kvm_register_device_ops(&kvm_flic_ops, KVM_DEV_TYPE_FLIC);
	if (rc) {
		pr_err("A FLIC registration call failed with rc=%d\n", rc);
		goto out;
	}

	rc = kvm_s390_gib_init(GAL_ISC);
	if (rc)
		goto out;

	return 0;

out:
	kvm_arch_exit();
	return rc;
}