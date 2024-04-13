void agdelcb(Agraph_t * g, void *obj, Agcbstack_t * cbstack)
{
    agobjfn_t fn;

    if (cbstack == NIL(Agcbstack_t *))
	return;
    agdelcb(g, obj, cbstack->prev);
    fn = NIL(agobjfn_t);
    switch (AGTYPE(obj)) {
    case AGRAPH:
	fn = cbstack->f->graph.del;
	break;
    case AGNODE:
	fn = cbstack->f->node.del;
	break;
    case AGEDGE:
	fn = cbstack->f->edge.del;
	break;
    }
    if (fn)
	fn(g, obj, cbstack->state);
}