int agcontains(Agraph_t* g, void* obj)
{
    Agraph_t* subg;

    if (agroot (g) != agroot (obj)) return 0;
    switch (AGTYPE(obj)) {
    case AGRAPH:
	subg = (Agraph_t *) obj;
	do {
	    if (subg == g) return 1;
	} while ((subg = agparent (subg)));
	return 0;
    case AGNODE: 
        return (agidnode(g, AGID(obj), 0) != 0);
    default:
        return (agsubedge(g, (Agedge_t *) obj, 0) != 0);
    }
}