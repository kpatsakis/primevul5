static void SFDSetEncMap(SplineFont *sf,int orig_pos,int enc) {
    EncMap *map = sf->map;

    if ( map==NULL )
return;

    if ( orig_pos>=map->backmax ) {
	int old = map->backmax;
	map->backmax = orig_pos+10;
	map->backmap = realloc(map->backmap,map->backmax*sizeof(int));
	memset(map->backmap+old,-1,(map->backmax-old)*sizeof(int));
    }
    if ( map->backmap[orig_pos] == -1 )		/* backmap will not be unique if multiple encodings come from same glyph */
	map->backmap[orig_pos] = enc;
    if ( enc>=map->encmax ) {
	int old = map->encmax;
	map->encmax = enc+10;
	map->map = realloc(map->map,map->encmax*sizeof(int));
	memset(map->map+old,-1,(map->encmax-old)*sizeof(int));
    }
    if ( enc>=map->enccount )
	map->enccount = enc+1;
    if ( enc!=-1 )
	map->map[enc] = orig_pos;
}