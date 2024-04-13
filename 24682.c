  Field *create_tmp_field_ex(TABLE *table, Tmp_field_src *src,
                             const Tmp_field_param *param)
  {

    /*
      create_tmp_field_ex() for this type of Items is called for:
      - CREATE TABLE ... SELECT
      - In ORDER BY: SELECT max(a) FROM t1 GROUP BY a ORDER BY 'const';
      - In CURSORS:
          DECLARE c CURSOR FOR SELECT 'test';
          OPEN c;
    */
    return tmp_table_field_from_field_type_maybe_null(table, src, param,
                                            type() == Item::NULL_ITEM);
  }