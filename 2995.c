
GF_Err subs_box_size(GF_Box *s)
{
	GF_SubSampleInformationBox *ptr = (GF_SubSampleInformationBox *) s;
	u32 entry_count, i;
	u16 subsample_count;

	// add 4 byte for entry_count
	ptr->size += 4;
	entry_count = gf_list_count(ptr->Samples);
	for (i=0; i<entry_count; i++) {
		GF_SubSampleInfoEntry *pSamp = (GF_SubSampleInfoEntry*) gf_list_get(ptr->Samples, i);
		subsample_count = gf_list_count(pSamp->SubSamples);
		// 4 byte for sample_delta, 2 byte for subsample_count
		// and 6 + (4 or 2) bytes for each subsample
		ptr->size += 4 + 2 + subsample_count * (6 + (ptr->version==1 ? 4 : 2));
	}
	return GF_OK;