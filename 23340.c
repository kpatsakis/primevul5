static void add_id_to_list(unsigned *head, unsigned short *list,
			   unsigned short id)
{
	list[id] = *head;
	*head = id;
}