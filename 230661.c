static void sev_asid_free(int asid)
{
	struct svm_cpu_data *sd;
	int cpu, pos;

	mutex_lock(&sev_bitmap_lock);

	pos = asid - 1;
	__set_bit(pos, sev_reclaim_asid_bitmap);

	for_each_possible_cpu(cpu) {
		sd = per_cpu(svm_data, cpu);
		sd->sev_vmcbs[pos] = NULL;
	}

	mutex_unlock(&sev_bitmap_lock);
}