static void SFDParseMathItem(FILE *sfd,SplineFont *sf,char *tok) {
    /* The first five characters of a math item's keyword will be "MATH:" */
    /*  the rest will be one of the entries in math_constants_descriptor */
    int i;
    struct MATH *math;

    if ( (math = sf->MATH) == NULL )
	math = sf->MATH = calloc(1,sizeof(struct MATH));
    for ( i=0; math_constants_descriptor[i].script_name!=NULL; ++i ) {
	char *name = math_constants_descriptor[i].script_name;
	int len = strlen( name );
	if ( strncmp(tok+5,name,len)==0 && tok[5+len] == ':' && tok[6+len]=='\0' ) {
	    int16 *pos = (int16 *) (((char *) (math)) + math_constants_descriptor[i].offset );
	    getsint(sfd,pos);
	    if ( math_constants_descriptor[i].devtab_offset != -1 ) {
		DeviceTable **devtab = (DeviceTable **) (((char *) (math)) + math_constants_descriptor[i].devtab_offset );
		*devtab = SFDReadDeviceTable(sfd,*devtab);
    break;
	    }
	}
    }
}