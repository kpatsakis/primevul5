void JOIN::set_allowed_join_cache_types()
{
  allowed_join_cache_types= 0;
  if (optimizer_flag(thd, OPTIMIZER_SWITCH_JOIN_CACHE_INCREMENTAL))
    allowed_join_cache_types|= JOIN_CACHE_INCREMENTAL_BIT;
  if (optimizer_flag(thd, OPTIMIZER_SWITCH_JOIN_CACHE_HASHED))
    allowed_join_cache_types|= JOIN_CACHE_HASHED_BIT;
  if (optimizer_flag(thd, OPTIMIZER_SWITCH_JOIN_CACHE_BKA))
    allowed_join_cache_types|= JOIN_CACHE_BKA_BIT;
  allowed_semijoin_with_cache=
    optimizer_flag(thd, OPTIMIZER_SWITCH_SEMIJOIN_WITH_CACHE);
  allowed_outer_join_with_cache=
    optimizer_flag(thd, OPTIMIZER_SWITCH_OUTER_JOIN_WITH_CACHE);
  max_allowed_join_cache_level= thd->variables.join_cache_level;
}