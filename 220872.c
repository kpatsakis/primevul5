dataiterator_seek(Dataiterator *di, int whence)
{
  if ((whence & DI_SEEK_STAY) != 0)
    di->rootlevel = di->nparents;
  switch (whence & ~DI_SEEK_STAY)
    {
    case DI_SEEK_CHILD:
      if (di->state != di_nextarrayelement)
	break;
      if ((whence & DI_SEEK_STAY) != 0)
	di->rootlevel = di->nparents + 1;	/* XXX: dangerous! */
      di->state = di_entersub;
      break;
    case DI_SEEK_PARENT:
      if (!di->nparents)
	{
	  di->state = di_bye;
	  break;
	}
      di->nparents--;
      if (di->rootlevel > di->nparents)
	di->rootlevel = di->nparents;
      di->dp = di->parents[di->nparents].dp;
      di->kv = di->parents[di->nparents].kv;
      di->keyp = di->parents[di->nparents].keyp;
      di->key = di->data->keys + *di->keyp;
      di->ddp = (unsigned char *)di->kv.str;
      di->keyname = di->keynames[di->nparents - di->rootlevel];
      di->state = di_nextarrayelement;
      break;
    case DI_SEEK_REWIND:
      if (!di->nparents)
	{
	  di->state = di_bye;
	  break;
	}
      di->dp = (unsigned char *)di->kv.parent->str;
      di->keyp = di->data->schemadata + di->data->schemata[di->kv.parent->id];
      di->state = di_enterschema;
      break;
    default:
      break;
    }
}