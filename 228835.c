static void SFDParseChainContext(FILE *sfd,SplineFont *sf,FPST *fpst, char *tok, int old) {
    int ch, i, j, k, temp;
    SplineFont *sli_sf = sf->cidmaster ? sf->cidmaster : sf;

    fpst->type = strnmatch(tok,"ContextPos",10)==0 ? pst_contextpos :
		strnmatch(tok,"ContextSub",10)==0 ? pst_contextsub :
		strnmatch(tok,"ChainPos",8)==0 ? pst_chainpos :
		strnmatch(tok,"ChainSub",8)==0 ? pst_chainsub : pst_reversesub;
    getname(sfd,tok);
    fpst->format = strmatch(tok,"glyph")==0 ? pst_glyphs :
		    strmatch(tok,"class")==0 ? pst_class :
		    strmatch(tok,"coverage")==0 ? pst_coverage : pst_reversecoverage;
    if ( old ) {
	fscanf(sfd, "%hu %hu", &((FPST1 *) fpst)->flags, &((FPST1 *) fpst)->script_lang_index );
	if ( ((FPST1 *) fpst)->script_lang_index>=((SplineFont1 *) sli_sf)->sli_cnt && ((FPST1 *) fpst)->script_lang_index!=SLI_NESTED ) {
	    static int complained=false;
	    if ( ((SplineFont1 *) sli_sf)->sli_cnt==0 )
		IError("'%c%c%c%c' has a script index out of bounds: %d\nYou MUST fix this manually",
			(((FPST1 *) fpst)->tag>>24), (((FPST1 *) fpst)->tag>>16)&0xff, (((FPST1 *) fpst)->tag>>8)&0xff, ((FPST1 *) fpst)->tag&0xff,
			((FPST1 *) fpst)->script_lang_index );
	    else if ( !complained )
		IError("'%c%c%c%c' has a script index out of bounds: %d",
			(((FPST1 *) fpst)->tag>>24), (((FPST1 *) fpst)->tag>>16)&0xff, (((FPST1 *) fpst)->tag>>8)&0xff, ((FPST1 *) fpst)->tag&0xff,
			((FPST1 *) fpst)->script_lang_index );
	    else
		IError("'%c%c%c%c' has a script index out of bounds: %d\n",
			(((FPST1 *) fpst)->tag>>24), (((FPST1 *) fpst)->tag>>16)&0xff, (((FPST1 *) fpst)->tag>>8)&0xff, ((FPST1 *) fpst)->tag&0xff,
			((FPST1 *) fpst)->script_lang_index );
	    if ( ((SplineFont1 *) sli_sf)->sli_cnt!=0 )
		((FPST1 *) fpst)->script_lang_index = ((SplineFont1 *) sli_sf)->sli_cnt-1;
	    complained = true;
	}
	while ( (ch=nlgetc(sfd))==' ' || ch=='\t' );
	if ( ch=='\'' ) {
	    ungetc(ch,sfd);
	    ((FPST1 *) fpst)->tag = gettag(sfd);
	} else
	    ungetc(ch,sfd);
    } else {
	fpst->subtable = SFFindLookupSubtableAndFreeName(sf,SFDReadUTF7Str(sfd));
        if ( !fpst->subtable )
            LogError(_("Missing Subtable definition found in chained context"));
        else
	    fpst->subtable->fpst = fpst;
    }
    fscanf(sfd, "%hu %hu %hu %hu", &fpst->nccnt, &fpst->bccnt, &fpst->fccnt, &fpst->rule_cnt );
    if ( fpst->nccnt!=0 || fpst->bccnt!=0 || fpst->fccnt!=0 ) {
	fpst->nclass = malloc(fpst->nccnt*sizeof(char *));
	fpst->nclassnames = calloc(fpst->nccnt,sizeof(char *));
	if ( fpst->nccnt!=0 ) fpst->nclass[0] = NULL;
	if ( fpst->bccnt!=0 || fpst->fccnt!=0 ) {
	    fpst->bclass = malloc(fpst->bccnt*sizeof(char *));
	    fpst->bclassnames = calloc(fpst->bccnt,sizeof(char *));
	    if (fpst->bccnt!=0 ) fpst->bclass[0] = NULL;
	    fpst->fclass = malloc(fpst->fccnt*sizeof(char *));
	    fpst->fclassnames = calloc(fpst->fccnt,sizeof(char *));
	    if (fpst->fccnt!=0 ) fpst->fclass[0] = NULL;
	}
    }

    for ( j=0; j<3; ++j ) {
	for ( i=1; i<(&fpst->nccnt)[j]; ++i ) {
	    getname(sfd,tok);
	    if ( i==1 && j==0 && strcmp(tok,"Class0:")==0 )
		i=0;
	    getint(sfd,&temp);
	    (&fpst->nclass)[j][i] = malloc(temp+1); (&fpst->nclass)[j][i][temp] = '\0';
	    nlgetc(sfd);	/* skip space */
	    fread((&fpst->nclass)[j][i],1,temp,sfd);
	}
    }

    fpst->rules = calloc(fpst->rule_cnt,sizeof(struct fpst_rule));
    for ( i=0; i<fpst->rule_cnt; ++i ) {
	switch ( fpst->format ) {
	  case pst_glyphs:
	    for ( j=0; j<3; ++j ) {
		getname(sfd,tok);
		getint(sfd,&temp);
		(&fpst->rules[i].u.glyph.names)[j] = malloc(temp+1);
		(&fpst->rules[i].u.glyph.names)[j][temp] = '\0';
		nlgetc(sfd);	/* skip space */
		fread((&fpst->rules[i].u.glyph.names)[j],1,temp,sfd);
	    }
	  break;
	  case pst_class:
	    fscanf( sfd, "%d %d %d", &fpst->rules[i].u.class.ncnt, &fpst->rules[i].u.class.bcnt, &fpst->rules[i].u.class.fcnt );
	    for ( j=0; j<3; ++j ) {
		getname(sfd,tok);
		(&fpst->rules[i].u.class.nclasses)[j] = malloc((&fpst->rules[i].u.class.ncnt)[j]*sizeof(uint16));
		for ( k=0; k<(&fpst->rules[i].u.class.ncnt)[j]; ++k ) {
		    getusint(sfd,&(&fpst->rules[i].u.class.nclasses)[j][k]);
		}
	    }
	  break;
	  case pst_coverage:
	  case pst_reversecoverage:
	    fscanf( sfd, "%d %d %d", &fpst->rules[i].u.coverage.ncnt, &fpst->rules[i].u.coverage.bcnt, &fpst->rules[i].u.coverage.fcnt );
	    for ( j=0; j<3; ++j ) {
		(&fpst->rules[i].u.coverage.ncovers)[j] = malloc((&fpst->rules[i].u.coverage.ncnt)[j]*sizeof(char *));
		for ( k=0; k<(&fpst->rules[i].u.coverage.ncnt)[j]; ++k ) {
		    getname(sfd,tok);
		    getint(sfd,&temp);
		    (&fpst->rules[i].u.coverage.ncovers)[j][k] = malloc(temp+1);
		    (&fpst->rules[i].u.coverage.ncovers)[j][k][temp] = '\0';
		    nlgetc(sfd);	/* skip space */
		    fread((&fpst->rules[i].u.coverage.ncovers)[j][k],1,temp,sfd);
		}
	    }
	  break;
	  default:
	  break;
	}
	switch ( fpst->format ) {
	  case pst_glyphs:
	  case pst_class:
	  case pst_coverage:
	    getint(sfd,&fpst->rules[i].lookup_cnt);
	    fpst->rules[i].lookups = malloc(fpst->rules[i].lookup_cnt*sizeof(struct seqlookup));
	    for ( j=k=0; j<fpst->rules[i].lookup_cnt; ++j ) {
		getname(sfd,tok);
		getint(sfd,&fpst->rules[i].lookups[j].seq);
		fpst->rules[i].lookups[k].lookup = SFD_ParseNestedLookup(sfd,sf,old);
		if ( fpst->rules[i].lookups[k].lookup!=NULL )
		    ++k;
	    }
	    fpst->rules[i].lookup_cnt = k;
	  break;
	  case pst_reversecoverage:
	    getname(sfd,tok);
	    getint(sfd,&temp);
	    fpst->rules[i].u.rcoverage.replacements = malloc(temp+1);
	    fpst->rules[i].u.rcoverage.replacements[temp] = '\0';
	    nlgetc(sfd);	/* skip space */
	    fread(fpst->rules[i].u.rcoverage.replacements,1,temp,sfd);
	  break;
	  default:
	  break;
	}
    }
    getname(sfd,tok);	/* EndFPST, or one of the ClassName tokens (in newer sfds) */
    while ( strcmp(tok,"ClassNames:")==0 || strcmp(tok,"BClassNames:")==0 ||
	    strcmp(tok,"FClassNames:")==0 ) {
	int which = strcmp(tok,"ClassNames:")==0 ? 0 :
		    strcmp(tok,"BClassNames:")==0 ? 1 : 2;
	int cnt = (&fpst->nccnt)[which];
	char **classnames = (&fpst->nclassnames)[which];
	int i;

	for ( i=0; i<cnt; ++i )
	    classnames[i] = SFDReadUTF7Str(sfd);
	getname(sfd,tok);	/* EndFPST, or one of the ClassName tokens (in newer sfds) */
    }

}