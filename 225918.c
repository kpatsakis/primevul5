add_mlist(struct mlist *mlp, struct magic_map *map, size_t idx)
{
	struct mlist *ml;

	if ((ml = CAST(struct mlist *, emalloc(sizeof(*ml)))) == NULL)
		return -1;

	ml->map = idx == 0 ? map : NULL;
	ml->magic = map->magic[idx];
	ml->nmagic = map->nmagic[idx];

	mlp->prev->next = ml;
	ml->prev = mlp->prev;
	ml->next = mlp;
	mlp->prev = ml;
	return 0;
}