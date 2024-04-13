static void SFDDumpValDevTab(FILE *sfd,ValDevTab *adjust) {
    if ( adjust==NULL )
return;
    fprintf( sfd, " [ddx=" ); SFDDumpDeviceTable(sfd,&adjust->xadjust);
    fprintf( sfd, " ddy=" ); SFDDumpDeviceTable(sfd,&adjust->yadjust);
    fprintf( sfd, " ddh=" ); SFDDumpDeviceTable(sfd,&adjust->xadv);
    fprintf( sfd, " ddv=" ); SFDDumpDeviceTable(sfd,&adjust->yadv);
    putc(']',sfd);
}