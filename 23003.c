static void kvm_rtas_get_xive(struct kvm_vcpu *vcpu, struct rtas_args *args)
{
	u32 irq, server, priority;
	int rc;

	if (be32_to_cpu(args->nargs) != 1 || be32_to_cpu(args->nret) != 3) {
		rc = -3;
		goto out;
	}

	irq = be32_to_cpu(args->args[0]);

	server = priority = 0;
	if (xics_on_xive())
		rc = kvmppc_xive_get_xive(vcpu->kvm, irq, &server, &priority);
	else
		rc = kvmppc_xics_get_xive(vcpu->kvm, irq, &server, &priority);
	if (rc) {
		rc = -3;
		goto out;
	}

	args->rets[1] = cpu_to_be32(server);
	args->rets[2] = cpu_to_be32(priority);
out:
	args->rets[0] = cpu_to_be32(rc);
}