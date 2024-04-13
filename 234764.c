uint find_shortest_key(TABLE *table, const key_map *usable_keys)
{
  double min_cost= DBL_MAX;
  uint best= MAX_KEY;
  if (!usable_keys->is_clear_all())
  {
    for (uint nr=0; nr < table->s->keys ; nr++)
    {
      if (usable_keys->is_set(nr))
      {
        double cost= table->file->keyread_time(nr, 1, table->file->records());
        if (cost < min_cost)
        {
          min_cost= cost;
          best=nr;
        }
        DBUG_ASSERT(best < MAX_KEY);
      }
    }
  }
  return best;
}