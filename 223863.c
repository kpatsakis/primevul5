static inline qDeqID getNextDeqID(qqueue_t *pQueue)
{
	ISOBJ_TYPE_assert(pQueue, qqueue);
	return pQueue->deqIDAdd++;
}