static void SFDParseMathValueRecord(FILE *sfd,int16 *value,DeviceTable **devtab) {
    getsint(sfd,value);
    *devtab = SFDReadDeviceTable(sfd,NULL);
}