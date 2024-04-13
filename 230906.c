u16 vcs_scr_readw(struct vc_data *vc, const u16 *org)
{
	if ((unsigned long)org == vc->vc_pos && softcursor_original != -1)
		return softcursor_original;
	return scr_readw(org);
}