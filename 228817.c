void SFD_DumpKerns( FILE *sfd, SplineChar *sc, int *newgids ) {
    KernPair *kp;
    int v;

    for ( v=0; v<2; ++v ) {
	kp = v ? sc->vkerns : sc->kerns;
	if ( kp!=NULL ) {
	    fprintf( sfd, v ? "VKerns2:" : "Kerns2:" );
	    for ( ; kp!=NULL; kp=kp->next )
		if ( !SFDOmit(kp->sc)) {
		    fprintf( sfd, " %d %d ",
			    newgids!=NULL?newgids[kp->sc->orig_pos]:kp->sc->orig_pos,
			    kp->off );
		    SFDDumpUTF7Str(sfd,kp->subtable->subtable_name);
		    if ( kp->adjust!=NULL ) putc(' ',sfd);
		    SFDDumpDeviceTable(sfd,kp->adjust);
		}
	    fprintf(sfd, "\n" );
	}
    }
}