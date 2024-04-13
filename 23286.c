static unsigned short get_id_from_list(unsigned *head, unsigned short *list)
{
	unsigned int id = *head;

	if (id != TX_LINK_NONE) {
		*head = list[id];
		list[id] = TX_LINK_NONE;
	}
	return id;
}