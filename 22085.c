destructSrv(ptcpsrv_t *pSrv)
{
	if(pSrv->ratelimiter != NULL)
		ratelimitDestruct(pSrv->ratelimiter);
	if(pSrv->pInputName != NULL)
		prop.Destruct(&pSrv->pInputName);
	pthread_mutex_destroy(&pSrv->mutSessLst);
	if(pSrv->pszInputName != NULL)
		free(pSrv->pszInputName);
	if(pSrv->port != NULL)
		free(pSrv->port);
	if(pSrv->path != NULL)
		free(pSrv->path);
	if(pSrv->lstnIP != NULL)
		free(pSrv->lstnIP);
	free(pSrv);
}