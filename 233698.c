void free_list(struct name_list *names)
{
	struct name_list *next;

	while (names) {
		next = names->next;
		free(names);
		names = next;
	}
}