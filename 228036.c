reset_last_used_map(mapblock_T *mp)
{
    if (last_used_map == mp)
	last_used_map = NULL;
}