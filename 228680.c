static void MMInferStuff(MMSet *mm) {
    int i,j;

    if ( mm==NULL )
return;
    if ( mm->apple ) {
	for ( i=0; i<mm->axis_count; ++i ) {
	    for ( j=0; j<mm->axismaps[i].points; ++j ) {
		real val = mm->axismaps[i].blends[j];
		if ( val == -1. )
		    mm->axismaps[i].min = mm->axismaps[i].designs[j];
		else if ( val==0 )
		    mm->axismaps[i].def = mm->axismaps[i].designs[j];
		else if ( val==1 )
		    mm->axismaps[i].max = mm->axismaps[i].designs[j];
	    }
	}
    }
}