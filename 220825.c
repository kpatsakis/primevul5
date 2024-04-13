solvabledata_fetch(Solvable *s, KeyValue *kv, Id keyname)
{
  kv->id = keyname;
  switch (keyname)
    {
    case SOLVABLE_NAME:
      kv->eof = 1;
      return &s->name;
    case SOLVABLE_ARCH:
      kv->eof = 1;
      return &s->arch;
    case SOLVABLE_EVR:
      kv->eof = 1;
      return &s->evr;
    case SOLVABLE_VENDOR:
      kv->eof = 1;
      return &s->vendor;
    case SOLVABLE_PROVIDES:
      kv->eof = 0;
      return s->provides ? s->repo->idarraydata + s->provides : 0;
    case SOLVABLE_OBSOLETES:
      kv->eof = 0;
      return s->obsoletes ? s->repo->idarraydata + s->obsoletes : 0;
    case SOLVABLE_CONFLICTS:
      kv->eof = 0;
      return s->conflicts ? s->repo->idarraydata + s->conflicts : 0;
    case SOLVABLE_REQUIRES:
      kv->eof = 0;
      return s->requires ? s->repo->idarraydata + s->requires : 0;
    case SOLVABLE_RECOMMENDS:
      kv->eof = 0;
      return s->recommends ? s->repo->idarraydata + s->recommends : 0;
    case SOLVABLE_SUPPLEMENTS:
      kv->eof = 0;
      return s->supplements ? s->repo->idarraydata + s->supplements : 0;
    case SOLVABLE_SUGGESTS:
      kv->eof = 0;
      return s->suggests ? s->repo->idarraydata + s->suggests : 0;
    case SOLVABLE_ENHANCES:
      kv->eof = 0;
      return s->enhances ? s->repo->idarraydata + s->enhances : 0;
    case RPM_RPMDBID:
      kv->eof = 1;
      return s->repo->rpmdbid ? s->repo->rpmdbid + (s - s->repo->pool->solvables - s->repo->start) : 0;
    default:
      return 0;
    }
}