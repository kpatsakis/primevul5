static void ASMReplaceTagsWithLookups(ASM *sm,SplineFont1 *sf) {
    int i;

    if ( sm->type != asm_context )
return;
    for ( i=0; i<sm->class_cnt*sm->state_cnt; ++i ) {
	if ( sm->state[i].u.context.mark_lookup!=NULL )
	    sm->state[i].u.context.mark_lookup = FindNestedLookupByTag(sf,(uint32) (intpt) (sm->state[i].u.context.mark_lookup) );
	if ( sm->state[i].u.context.cur_lookup!=NULL )
	    sm->state[i].u.context.cur_lookup = FindNestedLookupByTag(sf,(uint32) (intpt) (sm->state[i].u.context.cur_lookup) );
    }
}