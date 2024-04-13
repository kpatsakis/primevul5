GF_Err stbl_box_size(GF_Box *s)
{
	u32 pos=0;
	GF_SampleTableBox *ptr = (GF_SampleTableBox *)s;

	gf_isom_check_position(s, (GF_Box *)ptr->SampleDescription, &pos);
	gf_isom_check_position(s, (GF_Box *)ptr->TimeToSample, &pos);
	gf_isom_check_position(s, (GF_Box *)ptr->CompositionOffset, &pos);
	gf_isom_check_position(s, (GF_Box *)ptr->CompositionToDecode, &pos);
	gf_isom_check_position(s, (GF_Box *)ptr->SyncSample, &pos);
	gf_isom_check_position(s, (GF_Box *)ptr->ShadowSync, &pos);
	gf_isom_check_position(s, (GF_Box *)ptr->SampleToChunk, &pos);
	gf_isom_check_position(s, (GF_Box *)ptr->SampleSize, &pos);
	gf_isom_check_position(s, (GF_Box *)ptr->ChunkOffset, &pos);
	gf_isom_check_position(s, (GF_Box *)ptr->DegradationPriority, &pos);
	gf_isom_check_position(s, (GF_Box *)ptr->SampleDep, &pos);
	gf_isom_check_position(s, (GF_Box *)ptr->PaddingBits, &pos);

	if (ptr->sub_samples) {
		gf_isom_check_position_list(s, ptr->sub_samples, &pos);
	}
	if (ptr->sampleGroupsDescription) {
		gf_isom_check_position_list(s, ptr->sampleGroupsDescription, &pos);
	}
	if (ptr->sampleGroups) {
		gf_isom_check_position_list(s, ptr->sampleGroups, &pos);
	}
	if (ptr->sai_sizes) {
		gf_isom_check_position_list(s, ptr->sai_sizes, &pos);
	}
	if (ptr->sai_offsets) {
		gf_isom_check_position_list(s, ptr->sai_offsets, &pos);
	}
	return GF_OK;
}