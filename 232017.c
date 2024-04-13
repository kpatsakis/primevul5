void free_key_material(key_material_t* pkey)
{
	int i;

	if (!pkey)
	{
		return;
	}

	for (i = 0; i < TAG_MAX; i++)
	{
		if (pkey[i].big)
		{
			free(pkey[i].big);
		}
	}
}