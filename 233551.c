sbni_setup( char  *p )
{
	int  n, parm;

	if( *p++ != '(' )
		goto  bad_param;

	for( n = 0, parm = 0;  *p  &&  n < 8; ) {
		(*dest[ parm ])[ n ] = simple_strtol( p, &p, 0 );
		if( !*p  ||  *p == ')' )
			return 1;
		if( *p == ';' )
			++p, ++n, parm = 0;
		else if( *p++ != ',' )
			break;
		else
			if( ++parm >= 5 )
				break;
	}
bad_param:
	printk( KERN_ERR "Error in sbni kernel parameter!\n" );
	return 0;
}