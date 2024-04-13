static RIOMapRef *_mapref_from_map(RIOMap *map) {
	RIOMapRef *mapref = R_NEW (RIOMapRef);
	if (mapref) {
		mapref->id = map->id;
		mapref->ts = map->ts;
	}
	return mapref;
}