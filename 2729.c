pqFreeCommandQueue(PGcmdQueueEntry *queue)
{
	while (queue != NULL)
	{
		PGcmdQueueEntry *cur = queue;

		queue = cur->next;
		if (cur->query)
			free(cur->query);
		free(cur);
	}
}