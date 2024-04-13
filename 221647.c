static void override_cache_bits(struct cpuinfo_x86 *c)
{
	if (c->x86 != 6)
		return;

	switch (c->x86_model) {
	case INTEL_FAM6_NEHALEM:
	case INTEL_FAM6_WESTMERE:
	case INTEL_FAM6_SANDYBRIDGE:
	case INTEL_FAM6_IVYBRIDGE:
	case INTEL_FAM6_HASWELL:
	case INTEL_FAM6_HASWELL_L:
	case INTEL_FAM6_HASWELL_G:
	case INTEL_FAM6_BROADWELL:
	case INTEL_FAM6_BROADWELL_G:
	case INTEL_FAM6_SKYLAKE_L:
	case INTEL_FAM6_SKYLAKE:
	case INTEL_FAM6_KABYLAKE_L:
	case INTEL_FAM6_KABYLAKE:
		if (c->x86_cache_bits < 44)
			c->x86_cache_bits = 44;
		break;
	}
}