static void SFDParseStateMachine(FILE *sfd,SplineFont *sf,ASM *sm, char *tok,int old) {
    int i, temp;

    sm->type = strnmatch(tok,"MacIndic",8)==0 ? asm_indic :
		strnmatch(tok,"MacContext",10)==0 ? asm_context :
		strnmatch(tok,"MacLigature",11)==0 ? asm_lig :
		strnmatch(tok,"MacSimple",9)==0 ? asm_simple :
		strnmatch(tok,"MacKern",7)==0 ? asm_kern : asm_insert;
    if ( old ) {
	getusint(sfd,&((ASM1 *) sm)->feature);
	nlgetc(sfd);		/* Skip comma */
	getusint(sfd,&((ASM1 *) sm)->setting);
    } else {
	sm->subtable = SFFindLookupSubtableAndFreeName(sf,SFDReadUTF7Str(sfd));
	sm->subtable->sm = sm;
    }
    getusint(sfd,&sm->flags);
    getusint(sfd,&sm->class_cnt);
    getusint(sfd,&sm->state_cnt);

    sm->classes = malloc(sm->class_cnt*sizeof(char *));
    sm->classes[0] = sm->classes[1] = sm->classes[2] = sm->classes[3] = NULL;
    for ( i=4; i<sm->class_cnt; ++i ) {
	getname(sfd,tok);
	getint(sfd,&temp);
	sm->classes[i] = malloc(temp+1); sm->classes[i][temp] = '\0';
	nlgetc(sfd);	/* skip space */
	fread(sm->classes[i],1,temp,sfd);
    }

    sm->state = malloc(sm->class_cnt*sm->state_cnt*sizeof(struct asm_state));
    for ( i=0; i<sm->class_cnt*sm->state_cnt; ++i ) {
	getusint(sfd,&sm->state[i].next_state);
	getusint(sfd,&sm->state[i].flags);
	if ( sm->type == asm_context ) {
	    sm->state[i].u.context.mark_lookup = SFD_ParseNestedLookup(sfd,sf,old);
	    sm->state[i].u.context.cur_lookup = SFD_ParseNestedLookup(sfd,sf,old);
	} else if ( sm->type == asm_insert ) {
	    getint(sfd,&temp);
	    if ( temp==0 )
		sm->state[i].u.insert.mark_ins = NULL;
	    else {
		sm->state[i].u.insert.mark_ins = malloc(temp+1); sm->state[i].u.insert.mark_ins[temp] = '\0';
		nlgetc(sfd);	/* skip space */
		fread(sm->state[i].u.insert.mark_ins,1,temp,sfd);
	    }
	    getint(sfd,&temp);
	    if ( temp==0 )
		sm->state[i].u.insert.cur_ins = NULL;
	    else {
		sm->state[i].u.insert.cur_ins = malloc(temp+1); sm->state[i].u.insert.cur_ins[temp] = '\0';
		nlgetc(sfd);	/* skip space */
		fread(sm->state[i].u.insert.cur_ins,1,temp,sfd);
	    }
	} else if ( sm->type == asm_kern ) {
	    int j;
	    getint(sfd,&sm->state[i].u.kern.kcnt);
	    if ( sm->state[i].u.kern.kcnt!=0 )
		sm->state[i].u.kern.kerns = malloc(sm->state[i].u.kern.kcnt*sizeof(int16));
	    for ( j=0; j<sm->state[i].u.kern.kcnt; ++j ) {
		getint(sfd,&temp);
		sm->state[i].u.kern.kerns[j] = temp;
	    }
	}
    }
    getname(sfd,tok);			/* EndASM */
}