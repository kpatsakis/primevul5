sort_page_names (gconstpointer a,
                 gconstpointer b)
{
	const char *name_1, *name_2;
	gchar *key_1, *key_2;
	gboolean sort_last_1, sort_last_2;
	int compare;

	name_1 = * (const char **) a;
	name_2 = * (const char **) b;

	#define SORT_LAST_CHAR1 '.'
	#define SORT_LAST_CHAR2 '#'

	sort_last_1 = name_1[0] == SORT_LAST_CHAR1 || name_1[0] == SORT_LAST_CHAR2;
	sort_last_2 = name_2[0] == SORT_LAST_CHAR1 || name_2[0] == SORT_LAST_CHAR2;

	#undef SORT_LAST_CHAR1
	#undef SORT_LAST_CHAR2

	if (sort_last_1 && !sort_last_2)
	{
		compare = +1;
	}
	else if (!sort_last_1 && sort_last_2)
	{
		compare = -1;
	} 
	else
	{
		key_1 = g_utf8_collate_key_for_filename (name_1, -1);
		key_2 = g_utf8_collate_key_for_filename (name_2, -1);

		compare = strcmp (key_1, key_2);

		g_free (key_1);
		g_free (key_2);
	}

	return compare;
}