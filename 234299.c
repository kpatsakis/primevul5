Agraph_t *agroot(void* obj)
{
    // fixes CVE-2019-11023 by moving the problem to the caller :-)
    if (obj == 0) return NILgraph; 
    switch (AGTYPE(obj)) {
    case AGINEDGE:
    case AGOUTEDGE:
	return ((Agedge_t *) obj)->node->root;
    case AGNODE:
	return ((Agnode_t *) obj)->root;
    case AGRAPH:
	return ((Agraph_t *) obj)->root;
    default:			/* actually can't occur if only 2 bit tags */
	agerr(AGERR, "agroot of a bad object");
	return NILgraph;
    }
}