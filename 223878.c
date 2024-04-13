static rsRetVal qDestructLinkedList(qqueue_t __attribute__((unused)) *pThis)
{
	DEFiRet;

	queueDrain(pThis); /* discard any remaining queue entries */

	/* with the linked list type, there is nothing left to do here. The
	 * reason is that there are no dynamic elements for the list itself.
	 */

	RETiRet;
}