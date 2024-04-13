int maria_check_definition(MARIA_KEYDEF *t1_keyinfo,
                           MARIA_COLUMNDEF *t1_recinfo,
                           uint t1_keys, uint t1_recs,
                           MARIA_KEYDEF *t2_keyinfo,
                           MARIA_COLUMNDEF *t2_recinfo,
                           uint t2_keys, uint t2_recs, bool strict)
{
  uint i, j;
  DBUG_ENTER("maria_check_definition");
  if ((strict ? t1_keys != t2_keys : t1_keys > t2_keys))
  {
    DBUG_PRINT("error", ("Number of keys differs: t1_keys=%u, t2_keys=%u",
                         t1_keys, t2_keys));
    DBUG_RETURN(1);
  }
  if (t1_recs != t2_recs)
  {
    DBUG_PRINT("error", ("Number of recs differs: t1_recs=%u, t2_recs=%u",
                         t1_recs, t2_recs));
    DBUG_RETURN(1);
  }
  for (i= 0; i < t1_keys; i++)
  {
    HA_KEYSEG *t1_keysegs= t1_keyinfo[i].seg;
    HA_KEYSEG *t2_keysegs= t2_keyinfo[i].seg;
    if (t1_keyinfo[i].flag & HA_FULLTEXT && t2_keyinfo[i].flag & HA_FULLTEXT)
      continue;
    else if (t1_keyinfo[i].flag & HA_FULLTEXT ||
             t2_keyinfo[i].flag & HA_FULLTEXT)
    {
       DBUG_PRINT("error", ("Key %d has different definition", i));
       DBUG_PRINT("error", ("t1_fulltext= %d, t2_fulltext=%d",
                            MY_TEST(t1_keyinfo[i].flag & HA_FULLTEXT),
                            MY_TEST(t2_keyinfo[i].flag & HA_FULLTEXT)));
       DBUG_RETURN(1);
    }
    if (t1_keyinfo[i].flag & HA_SPATIAL && t2_keyinfo[i].flag & HA_SPATIAL)
      continue;
    else if (t1_keyinfo[i].flag & HA_SPATIAL ||
             t2_keyinfo[i].flag & HA_SPATIAL)
    {
       DBUG_PRINT("error", ("Key %d has different definition", i));
       DBUG_PRINT("error", ("t1_spatial= %d, t2_spatial=%d",
                            MY_TEST(t1_keyinfo[i].flag & HA_SPATIAL),
                            MY_TEST(t2_keyinfo[i].flag & HA_SPATIAL)));
       DBUG_RETURN(1);
    }
    if (t1_keyinfo[i].keysegs != t2_keyinfo[i].keysegs ||
        t1_keyinfo[i].key_alg != t2_keyinfo[i].key_alg)
    {
      DBUG_PRINT("error", ("Key %d has different definition", i));
      DBUG_PRINT("error", ("t1_keysegs=%d, t1_key_alg=%d",
                           t1_keyinfo[i].keysegs, t1_keyinfo[i].key_alg));
      DBUG_PRINT("error", ("t2_keysegs=%d, t2_key_alg=%d",
                           t2_keyinfo[i].keysegs, t2_keyinfo[i].key_alg));
      DBUG_RETURN(1);
    }
    for (j=  t1_keyinfo[i].keysegs; j--;)
    {
      uint8 t1_keysegs_j__type= t1_keysegs[j].type;
      /*
        Table migration from 4.1 to 5.1. In 5.1 a *TEXT key part is
        always HA_KEYTYPE_VARTEXT2. In 4.1 we had only the equivalent of
        HA_KEYTYPE_VARTEXT1. Since we treat both the same on MyISAM
        level, we can ignore a mismatch between these types.
      */
      if ((t1_keysegs[j].flag & HA_BLOB_PART) &&
          (t2_keysegs[j].flag & HA_BLOB_PART))
      {
        if ((t1_keysegs_j__type == HA_KEYTYPE_VARTEXT2) &&
            (t2_keysegs[j].type == HA_KEYTYPE_VARTEXT1))
          t1_keysegs_j__type= HA_KEYTYPE_VARTEXT1; /* purecov: tested */
        else if ((t1_keysegs_j__type == HA_KEYTYPE_VARBINARY2) &&
                 (t2_keysegs[j].type == HA_KEYTYPE_VARBINARY1))
          t1_keysegs_j__type= HA_KEYTYPE_VARBINARY1; /* purecov: inspected */
      }

      if (t1_keysegs_j__type != t2_keysegs[j].type ||
          t1_keysegs[j].language != t2_keysegs[j].language ||
          t1_keysegs[j].null_bit != t2_keysegs[j].null_bit ||
          t1_keysegs[j].length != t2_keysegs[j].length)
      {
        DBUG_PRINT("error", ("Key segment %d (key %d) has different "
                             "definition", j, i));
        DBUG_PRINT("error", ("t1_type=%d, t1_language=%d, t1_null_bit=%d, "
                             "t1_length=%d",
                             t1_keysegs[j].type, t1_keysegs[j].language,
                             t1_keysegs[j].null_bit, t1_keysegs[j].length));
        DBUG_PRINT("error", ("t2_type=%d, t2_language=%d, t2_null_bit=%d, "
                             "t2_length=%d",
                             t2_keysegs[j].type, t2_keysegs[j].language,
                             t2_keysegs[j].null_bit, t2_keysegs[j].length));

        DBUG_RETURN(1);
      }
    }
  }

  for (i= 0; i < t1_recs; i++)
  {
    MARIA_COLUMNDEF *t1_rec= &t1_recinfo[i];
    MARIA_COLUMNDEF *t2_rec= &t2_recinfo[i];
    /*
      FIELD_SKIP_ZERO can be changed to FIELD_NORMAL in maria_create,
      see NOTE1 in ma_create.c
    */
    if ((t1_rec->type != t2_rec->type &&
         !(t1_rec->type == (int) FIELD_SKIP_ZERO &&
           t1_rec->length == 1 &&
           t2_rec->type == (int) FIELD_NORMAL)) ||
        t1_rec->length != t2_rec->length ||
        t1_rec->null_bit != t2_rec->null_bit)
    {
      DBUG_PRINT("error", ("Field %d has different definition", i));
      DBUG_PRINT("error", ("t1_type=%d, t1_length=%d, t1_null_bit=%d",
                           t1_rec->type, t1_rec->length, t1_rec->null_bit));
      DBUG_PRINT("error", ("t2_type=%d, t2_length=%d, t2_null_bit=%d",
                           t2_rec->type, t2_rec->length, t2_rec->null_bit));
      DBUG_RETURN(1);
    }
  }
  DBUG_RETURN(0);
}