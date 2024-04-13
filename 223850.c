ShutdownWorkers(qqueue_t *pThis)
{
	DEFiRet;

	ISOBJ_TYPE_assert(pThis, qqueue);
	ASSERT(pThis->pqParent == NULL); /* detect invalid calling sequence */

	DBGOPRINT((obj_t*) pThis, "initiating worker thread shutdown sequence\n");

	CHKiRet(tryShutdownWorkersWithinQueueTimeout(pThis));

	if(getPhysicalQueueSize(pThis) > 0) {
		CHKiRet(tryShutdownWorkersWithinActionTimeout(pThis));
	}

	CHKiRet(cancelWorkers(pThis));

	/* ... finally ... all worker threads have terminated :-)
	 * Well, more precisely, they *are in termination*. Some cancel cleanup handlers
	 * may still be running. Note that the main queue's DA worker may still be running.
	 */
	DBGOPRINT((obj_t*) pThis, "worker threads terminated, remaining queue size log %d, phys %d.\n",
		  getLogicalQueueSize(pThis), getPhysicalQueueSize(pThis));

finalize_it:
	RETiRet;
}