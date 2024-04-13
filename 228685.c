void SFDDumpCharStartingMarker(FILE *sfd,SplineChar *sc) {
    if ( AllAscii(sc->name))
	fprintf(sfd, "StartChar: %s\n", sc->name );
    else {
	fprintf(sfd, "StartChar: " );
	SFDDumpUTF7Str(sfd,sc->name);
	putc('\n',sfd);
    }
}