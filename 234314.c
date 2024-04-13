void aginitcb(Agraph_t * g, void *obj, Agcbstack_t * cbstack)
{
    agobjfn_t fn;

    if (cbstack == NIL(Agcbstack_t *))
	return;
    aginitcb(g, obj, cbstack->prev);
    fn = NIL(agobjfn_t);
    switch (AGTYPE(obj)) {
    case AGRAPH:
	fn = cbstack->f->graph.ins;
	break;
    case AGNODE:
	fn = cbstack->f->node.ins;
	break;
    case AGEDGE:
	fn = cbstack->f->edge.ins;
	break;
    }
    if (fn)
	fn(g, obj, cbstack->state);
}