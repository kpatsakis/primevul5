apprentice_unmap(struct magic_map *map)
{
	if (map == NULL)
		return;
	if (map->p != php_magic_database) {
		if (map->p == NULL) {
			int j;
			for (j = 0; j < MAGIC_SETS; j++) {
				if (map->magic[j]) {
					efree(map->magic[j]);
				}
			}
		} else {
			efree(map->p);
		}
	}
	efree(map);
}