destructSess(ptcpsess_t *pSess)
{
	free(pSess->pMsg);
	free(pSess->epd);
	prop.Destruct(&pSess->peerName);
	prop.Destruct(&pSess->peerIP);
	/* TODO: make these inits compile-time switch depending: */
	pSess->pMsg = NULL;
	pSess->epd = NULL;
	free(pSess);
}