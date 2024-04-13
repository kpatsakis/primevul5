void Predicant_to_list_comparator::
       detect_unique_handlers(Type_handler_hybrid_field_type *compatible,
                              uint *unique_count,
                              uint *found_types)
{
  *unique_count= 0;
  *found_types= 0;
  for (uint i= 0; i < m_comparator_count; i++)
  {
    uint idx;
    if (find_handler(&idx, m_comparators[i].m_handler, i))
    {
      m_comparators[i].m_handler_index= i; // New unique handler
      (*unique_count)++;
      (*found_types)|= 1U << m_comparators[i].m_handler->cmp_type();
      compatible->set_handler(m_comparators[i].m_handler);
    }
    else
    {
      m_comparators[i].m_handler_index= idx; // Non-unique handler
    }
  }
}