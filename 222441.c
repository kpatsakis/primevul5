static void sca_copy_b_to_e(struct esca_block *d, struct bsca_block *s)
{
	int i;

	d->ipte_control = s->ipte_control;
	d->mcn[0] = s->mcn;
	for (i = 0; i < KVM_S390_BSCA_CPU_SLOTS; i++)
		sca_copy_entry(&d->cpu[i], &s->cpu[i]);
}