static struct _vcs_dpi_soc_bounding_box_st *get_asic_rev_soc_bb(
	uint32_t hw_internal_rev)
{
	if (ASICREV_IS_NAVI12_P(hw_internal_rev))
		return &dcn2_0_nv12_soc;

	return &dcn2_0_soc;
}