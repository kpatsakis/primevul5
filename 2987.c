GF_Err stsc_box_read(GF_Box *s, GF_BitStream *bs)
{
	u32 i;
	GF_SampleToChunkBox *ptr = (GF_SampleToChunkBox *)s;

	ISOM_DECREASE_SIZE(ptr, 4);
	ptr->nb_entries = gf_bs_read_u32(bs);

	if (ptr->nb_entries > ptr->size / 12 || (u64)ptr->nb_entries > (u64)SIZE_MAX/sizeof(GF_StscEntry)) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Invalid number of entries %d in stsc\n", ptr->nb_entries));
		return GF_ISOM_INVALID_FILE;
	}

	ptr->alloc_size = ptr->nb_entries;
	ptr->entries = NULL;
	if (ptr->nb_entries) {
		ptr->entries = gf_malloc(sizeof(GF_StscEntry)*ptr->alloc_size);
		if (!ptr->entries) return GF_OUT_OF_MEM;
	}

	for (i = 0; i < ptr->nb_entries; i++) {
		ptr->entries[i].firstChunk = gf_bs_read_u32(bs);
		ptr->entries[i].samplesPerChunk = gf_bs_read_u32(bs);
		ptr->entries[i].sampleDescriptionIndex = gf_bs_read_u32(bs);
		ptr->entries[i].isEdited = 0;
		ptr->entries[i].nextChunk = 0;
		if (!ptr->entries[i].firstChunk) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] invalid first chunk 0 in stsc entry\n", ptr->nb_entries));
			return GF_ISOM_INVALID_FILE;
		}

		//update the next chunk in the previous entry
		if (i) ptr->entries[i-1].nextChunk = ptr->entries[i].firstChunk;
	}
	ptr->currentIndex = 0;
	ptr->firstSampleInCurrentChunk = 0;
	ptr->currentChunk = 0;
	ptr->ghostNumber = 0;
	return GF_OK;
}