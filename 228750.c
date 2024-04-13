static void SFDDumpTtfInstrs(FILE *sfd,SplineChar *sc)
{
    SFDDumpTtfInstrsExplicit( sfd, sc->ttf_instrs,sc->ttf_instrs_len );
}