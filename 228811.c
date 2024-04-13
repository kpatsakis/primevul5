static void SFDSizeMap(EncMap *map,int glyphcnt,int enccnt) {
    if ( glyphcnt>map->backmax ) {
	map->backmap = realloc(map->backmap,glyphcnt*sizeof(int));
	memset(map->backmap+map->backmax,-1,(glyphcnt-map->backmax)*sizeof(int));
	map->backmax = glyphcnt;
    }
    if ( enccnt>map->encmax ) {
	map->map = realloc(map->map,enccnt*sizeof(int));
	memset(map->map+map->backmax,-1,(enccnt-map->encmax)*sizeof(int));
	map->encmax = map->enccount = enccnt;
    }
}