static int SFDGetBitmapProps(FILE *sfd,BDFFont *bdf,char *tok) {
    int pcnt;
    int i;

    if ( getint(sfd,&pcnt)!=1 || pcnt<=0 )
return( 0 );
    bdf->prop_cnt = pcnt;
    bdf->props = malloc(pcnt*sizeof(BDFProperties));
    for ( i=0; i<pcnt; ++i ) {
	if ( getname(sfd,tok)!=1 )
    break;
	if ( strcmp(tok,"BDFEndProperties")==0 )
    break;
	bdf->props[i].name = copy(tok);
	getint(sfd,&bdf->props[i].type);
	switch ( bdf->props[i].type&~prt_property ) {
	  case prt_int: case prt_uint:
	    getint(sfd,&bdf->props[i].u.val);
	  break;
	  case prt_string: case prt_atom:
	    geteol(sfd,tok);
	    if ( tok[strlen(tok)-1]=='"' ) tok[strlen(tok)-1] = '\0';
	    bdf->props[i].u.str = copy(tok[0]=='"'?tok+1:tok);
	  break;
	  default:
	  break;
	}
    }
    bdf->prop_cnt = i;
return( 1 );
}