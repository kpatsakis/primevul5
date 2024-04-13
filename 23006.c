static void kvm_rtas_set_xive(struct kvm_vcpu *vcpu, struct rtas_args *args)
{
	u32 irq, server, priority;
	int rc;

	if (be32_to_cpu(args->nargs) != 3 || be32_to_cpu(args->nret) != 1) {
		rc = -3;
		goto out;
	}

	irq = be32_to_cpu(args->args[0]);
	server = be32_to_cpu(args->args[1]);
	priority = be32_to_cpu(args->args[2]);

	if (xics_on_xive())
		rc = kvmppc_xive_set_xive(vcpu->kvm, irq, server, priority);
	else
		rc = kvmppc_xics_set_xive(vcpu->kvm, irq, server, priority);
	if (rc)
		rc = -3;
out:
	args->rets[0] = cpu_to_be32(rc);
}