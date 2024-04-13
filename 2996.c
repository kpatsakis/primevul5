GF_Err audio_sample_entry_box_size(GF_Box *s)
{
	u32 pos=0;
	GF_MPEGAudioSampleEntryBox *ptr = (GF_MPEGAudioSampleEntryBox *)s;
	gf_isom_audio_sample_entry_size((GF_AudioSampleEntryBox*)s);
	if (ptr->qtff_mode)
		return GF_OK;

	gf_isom_check_position(s, (GF_Box *)ptr->esd, &pos);
	gf_isom_check_position(s, (GF_Box *)ptr->cfg_3gpp, &pos);
	gf_isom_check_position(s, (GF_Box *)ptr->cfg_opus, &pos);
	gf_isom_check_position(s, (GF_Box *)ptr->cfg_ac3, &pos);
	gf_isom_check_position(s, (GF_Box *)ptr->cfg_flac, &pos);
	gf_isom_check_position(s, (GF_Box *)ptr->cfg_mlp, &pos);
	return GF_OK;
}