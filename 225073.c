int handle_dlpar_errorlog(struct pseries_hp_errorlog *hp_elog)
{
	int rc;

	/* pseries error logs are in BE format, convert to cpu type */
	switch (hp_elog->id_type) {
	case PSERIES_HP_ELOG_ID_DRC_COUNT:
		hp_elog->_drc_u.drc_count =
				be32_to_cpu(hp_elog->_drc_u.drc_count);
		break;
	case PSERIES_HP_ELOG_ID_DRC_INDEX:
		hp_elog->_drc_u.drc_index =
				be32_to_cpu(hp_elog->_drc_u.drc_index);
		break;
	case PSERIES_HP_ELOG_ID_DRC_IC:
		hp_elog->_drc_u.ic.count =
				be32_to_cpu(hp_elog->_drc_u.ic.count);
		hp_elog->_drc_u.ic.index =
				be32_to_cpu(hp_elog->_drc_u.ic.index);
	}

	switch (hp_elog->resource) {
	case PSERIES_HP_ELOG_RESOURCE_MEM:
		rc = dlpar_memory(hp_elog);
		break;
	case PSERIES_HP_ELOG_RESOURCE_CPU:
		rc = dlpar_cpu(hp_elog);
		break;
	case PSERIES_HP_ELOG_RESOURCE_PMEM:
		rc = dlpar_hp_pmem(hp_elog);
		break;

	default:
		pr_warn_ratelimited("Invalid resource (%d) specified\n",
				    hp_elog->resource);
		rc = -EINVAL;
	}

	return rc;
}