xmlXPathCacheNewNodeSet(xmlXPathContextPtr ctxt, xmlNodePtr val)
{
    if ((ctxt != NULL) && (ctxt->cache)) {
	xmlXPathContextCachePtr cache = (xmlXPathContextCachePtr) ctxt->cache;

	if ((cache->nodesetObjs != NULL) &&
	    (cache->nodesetObjs->number != 0))
	{
	    xmlXPathObjectPtr ret;
	    /*
	    * Use the nodset-cache.
	    */
	    ret = (xmlXPathObjectPtr)
		cache->nodesetObjs->items[--cache->nodesetObjs->number];
	    ret->type = XPATH_NODESET;
	    ret->boolval = 0;
	    if (val) {
		if ((ret->nodesetval->nodeMax == 0) ||
		    (val->type == XML_NAMESPACE_DECL))
		{
		    xmlXPathNodeSetAddUnique(ret->nodesetval, val);
		} else {
		    ret->nodesetval->nodeTab[0] = val;
		    ret->nodesetval->nodeNr = 1;
		}
	    }
#ifdef XP_DEBUG_OBJ_USAGE
	    xmlXPathDebugObjUsageRequested(ctxt, XPATH_NODESET);
#endif
	    return(ret);
	} else if ((cache->miscObjs != NULL) &&
	    (cache->miscObjs->number != 0))
	{
	    xmlXPathObjectPtr ret;
	    /*
	    * Fallback to misc-cache.
	    */

	    ret = (xmlXPathObjectPtr)
		cache->miscObjs->items[--cache->miscObjs->number];

	    ret->type = XPATH_NODESET;
	    ret->boolval = 0;
	    ret->nodesetval = xmlXPathNodeSetCreate(val);
	    if (ret->nodesetval == NULL) {
		ctxt->lastError.domain = XML_FROM_XPATH;
		ctxt->lastError.code = XML_ERR_NO_MEMORY;
		return(NULL);
	    }
#ifdef XP_DEBUG_OBJ_USAGE
	    xmlXPathDebugObjUsageRequested(ctxt, XPATH_NODESET);
#endif
	    return(ret);
	}
    }
    return(xmlXPathNewNodeSet(val));
}