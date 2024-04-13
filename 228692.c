static int TTFFeatureIndex( uint32 tag, struct table_ordering *ord ) {
    /* This is the order in which features should be executed */
    int cnt = 0;

    if ( ord!=NULL ) {
	for ( cnt=0; ord->ordered_features[cnt]!=0; ++cnt )
	    if ( ord->ordered_features[cnt]==tag )
	break;
return( cnt );
    }

    cnt+=2;

    switch ( tag ) {
/* GSUB ordering */
      case CHR('c','c','m','p'):	/* Must be first? */
return( cnt-2 );
      case CHR('l','o','c','l'):	/* Language dependent letter forms (serbian uses some different glyphs than russian) */
return( cnt-1 );
      case CHR('i','s','o','l'):
return( cnt );
      case CHR('j','a','l','t'):		/* must come after 'isol' */
return( cnt+1 );
      case CHR('f','i','n','a'):
return( cnt+2 );
      case CHR('f','i','n','2'):
      case CHR('f','a','l','t'):		/* must come after 'fina' */
return( cnt+3 );
      case CHR('f','i','n','3'):
return( cnt+4 );
      case CHR('m','e','d','i'):
return( cnt+5 );
      case CHR('m','e','d','2'):
return( cnt+6 );
      case CHR('i','n','i','t'):
return( cnt+7 );

      case CHR('r','t','l','a'):
return( cnt+100 );
      case CHR('s','m','c','p'): case CHR('c','2','s','c'):
return( cnt+200 );

      case CHR('r','l','i','g'):
return( cnt+300 );
      case CHR('c','a','l','t'):
return( cnt+301 );
      case CHR('l','i','g','a'):
return( cnt+302 );
      case CHR('d','l','i','g'): case CHR('h','l','i','g'):
return( cnt+303 );
      case CHR('c','s','w','h'):
return( cnt+304 );
      case CHR('m','s','e','t'):
return( cnt+305 );

      case CHR('f','r','a','c'):
return( cnt+306 );

/* Indic processing */
      case CHR('n','u','k','t'):
      case CHR('p','r','e','f'):
return( cnt+301 );
      case CHR('a','k','h','n'):
return( cnt+302 );
      case CHR('r','p','h','f'):
return( cnt+303 );
      case CHR('b','l','w','f'):
return( cnt+304 );
      case CHR('h','a','l','f'):
      case CHR('a','b','v','f'):
return( cnt+305 );
      case CHR('p','s','t','f'):
return( cnt+306 );
      case CHR('v','a','t','u'):
return( cnt+307 );

      case CHR('p','r','e','s'):
return( cnt+310 );
      case CHR('b','l','w','s'):
return( cnt+311 );
      case CHR('a','b','v','s'):
return( cnt+312 );
      case CHR('p','s','t','s'):
return( cnt+313 );
      case CHR('c','l','i','g'):
return( cnt+314 );
      
      case CHR('h','a','l','n'):
return( cnt+320 );
/* end indic ordering */

      case CHR('a','f','r','c'):
      case CHR('l','j','m','o'):
      case CHR('v','j','m','o'):
return( cnt+350 );
      case CHR('v','r','t','2'): case CHR('v','e','r','t'):
return( cnt+1010 );		/* Documented to come last */

/* Unknown things come after everything but vert/vrt2 */
      default:
return( cnt+1000 );

    }
}