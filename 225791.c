xmlXPathNodeTrailingSorted (xmlNodeSetPtr nodes, xmlNodePtr node) {
    int i, l;
    xmlNodePtr cur;
    xmlNodeSetPtr ret;

    if (node == NULL)
	return(nodes);

    ret = xmlXPathNodeSetCreate(NULL);
    if (ret == NULL)
        return(ret);
    if (xmlXPathNodeSetIsEmpty(nodes) ||
	(!xmlXPathNodeSetContains(nodes, node)))
	return(ret);

    l = xmlXPathNodeSetGetLength(nodes);
    for (i = l - 1; i >= 0; i--) {
	cur = xmlXPathNodeSetItem(nodes, i);
	if (cur == node)
	    break;
	if (xmlXPathNodeSetAddUnique(ret, cur) < 0)
	    break;
    }
    xmlXPathNodeSetSort(ret);	/* bug 413451 */
    return(ret);
}