void agupdcb(Agraph_t * g, void *obj, Agsym_t * sym, Agcbstack_t * cbstack)
{
    agobjupdfn_t fn;

    if (cbstack == NIL(Agcbstack_t *))
	return;
    agupdcb(g, obj, sym, cbstack->prev);
    fn = NIL(agobjupdfn_t);
    switch (AGTYPE(obj)) {
    case AGRAPH:
	fn = cbstack->f->graph.mod;
	break;
    case AGNODE:
	fn = cbstack->f->node.mod;
	break;
    case AGEDGE:
	fn = cbstack->f->edge.mod;
	break;
    }
    if (fn)
	fn(g, obj, cbstack->state, sym);
}