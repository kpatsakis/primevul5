void SFDGetKerns( FILE *sfd, SplineChar *sc, char* ttok ) {
    struct splinefont * sf = sc->parent;
    char tok[2001], ch;
    uint32 script = 0;
    SplineFont *sli_sf = sf->cidmaster ? sf->cidmaster : sf;

    strncpy(tok,ttok,sizeof(tok)-1);
    tok[2000]=0;

    if( strmatch(tok,"Kerns2:")==0 ||
	strmatch(tok,"VKerns2:")==0 ) {
	    KernPair *kp, *last=NULL;
	    int isv = *tok=='V';
	    int off, index;
	    struct lookup_subtable *sub;
	    int kernCount = 0;
	    if ( sf->sfd_version<2 )
		LogError(_("Found an new style kerning pair inside a version 1 (or lower) sfd file.\n") );
	    while ( fscanf(sfd,"%d %d", &index, &off )==2 ) {
		sub = SFFindLookupSubtableAndFreeName(sf,SFDReadUTF7Str(sfd));
		if ( sub==NULL ) {
		    LogError(_("KernPair with no subtable name.\n"));
	    	    break;
		}
		kernCount++;
		kp = chunkalloc(sizeof(KernPair1));
		kp->sc = (SplineChar *) (intpt) index;
		kp->kcid = true;
		kp->off = off;
		kp->subtable = sub;
		kp->next = NULL;
		while ( (ch=nlgetc(sfd))==' ' );
		ungetc(ch,sfd);
		if ( ch=='{' ) {
		    kp->adjust = SFDReadDeviceTable(sfd, NULL);
		}
		if ( last != NULL )
		    last->next = kp;
		else if ( isv )
		    sc->vkerns = kp;
		else
		    sc->kerns = kp;
		last = kp;
	    }
	    if( !kernCount ) {
//		printf("SFDGetKerns() have a BLANK KERN\n");
		sc->kerns = 0;
	    }
    } else if ( strmatch(tok,"Kerns:")==0 ||
		strmatch(tok,"KernsSLI:")==0 ||
		strmatch(tok,"KernsSLIF:")==0 ||
		strmatch(tok,"VKernsSLIF:")==0 ||
		strmatch(tok,"KernsSLIFO:")==0 ||
		strmatch(tok,"VKernsSLIFO:")==0 ) {
	    KernPair1 *kp, *last=NULL;
	    int index, off, sli, flags=0;
	    int hassli = (strmatch(tok,"KernsSLI:")==0);
	    int isv = *tok=='V';
	    int has_orig = strstr(tok,"SLIFO:")!=NULL;
	    if ( sf->sfd_version>=2 ) {
		IError( "Found an old style kerning pair inside a version 2 (or higher) sfd file." );
exit(1);
	    }
	    if ( strmatch(tok,"KernsSLIF:")==0 || strmatch(tok,"KernsSLIFO:")==0 ||
		    strmatch(tok,"VKernsSLIF:")==0 || strmatch(tok,"VKernsSLIFO:")==0 )
		hassli=2;
	    while ( (hassli==1 && fscanf(sfd,"%d %d %d", &index, &off, &sli )==3) ||
		    (hassli==2 && fscanf(sfd,"%d %d %d %d", &index, &off, &sli, &flags )==4) ||
		    (hassli==0 && fscanf(sfd,"%d %d", &index, &off )==2) ) {
		if ( !hassli )
		    sli = SFFindBiggestScriptLangIndex(sli_sf,
			    script!=0?script:SCScriptFromUnicode(sc),DEFAULT_LANG);
		if ( sli>=((SplineFont1 *) sli_sf)->sli_cnt && sli!=SLI_NESTED) {
		    static int complained=false;
		    if ( !complained )
			IError("'%s' in %s has a script index out of bounds: %d",
				isv ? "vkrn" : "kern",
				sc->name, sli );
		    sli = SFFindBiggestScriptLangIndex(sli_sf,
			    SCScriptFromUnicode(sc),DEFAULT_LANG);
		    complained = true;
		}
		kp = chunkalloc(sizeof(KernPair1));
		kp->kp.sc = (SplineChar *) (intpt) index;
		kp->kp.kcid = has_orig;
		kp->kp.off = off;
		kp->sli = sli;
		kp->flags = flags;
		kp->kp.next = NULL;
		while ( (ch=nlgetc(sfd))==' ' );
		ungetc(ch,sfd);
		if ( ch=='{' ) {
		    kp->kp.adjust = SFDReadDeviceTable(sfd, NULL);
		}
		if ( last != NULL )
		    last->kp.next = (KernPair *) kp;
		else if ( isv )
		    sc->vkerns = (KernPair *) kp;
		else
		    sc->kerns = (KernPair *) kp;
		last = kp;
	    }
    } else {
	return;
    }

    // we matched something, grab the next top level token to ttok
    getname( sfd, ttok );
}