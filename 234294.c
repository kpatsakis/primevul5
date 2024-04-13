int agrename(Agobj_t * obj, char *newname)
{
    Agraph_t *g;
    IDTYPE old_id, new_id;

    switch (AGTYPE(obj)) {
    case AGRAPH:
	old_id = AGID(obj);
	g = agraphof(obj);
	/* can we reserve the id corresponding to newname? */
	if (agmapnametoid(agroot(g), AGTYPE(obj), newname,
			  &new_id, FALSE) == 0)
	    return FAILURE;
	if (new_id == old_id)
	    return SUCCESS;
	if (agmapnametoid(agroot(g), AGTYPE(obj), newname,
			  &new_id, TRUE) == 0)
	    return FAILURE;
        /* obj* is unchanged, so no need to re agregister() */
	if (agparent(g) && agidsubg(agparent(g), new_id, 0))
	    return FAILURE;
	agfreeid(g, AGRAPH, old_id);
	AGID(g) = new_id;
	break;
    case AGNODE:
	return agrelabel_node((Agnode_t *) obj, newname);
	agrename(obj, newname);
	break;
    case AGINEDGE:
    case AGOUTEDGE:
	return FAILURE;
    }
    return SUCCESS;
}