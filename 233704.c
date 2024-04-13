int max_name_length(struct name_list *names)
{
	int max_len = 0;
	while (names != NULL) {
		struct name_list *next = names->next;
		int len = strlen(names->name);

		if (len > max_len)
			max_len = len;
		names = next;
	}
	return max_len;
}