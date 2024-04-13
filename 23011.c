static void kvm_rtas_int_on(struct kvm_vcpu *vcpu, struct rtas_args *args)
{
	u32 irq;
	int rc;

	if (be32_to_cpu(args->nargs) != 1 || be32_to_cpu(args->nret) != 1) {
		rc = -3;
		goto out;
	}

	irq = be32_to_cpu(args->args[0]);

	if (xics_on_xive())
		rc = kvmppc_xive_int_on(vcpu->kvm, irq);
	else
		rc = kvmppc_xics_int_on(vcpu->kvm, irq);
	if (rc)
		rc = -3;
out:
	args->rets[0] = cpu_to_be32(rc);
}