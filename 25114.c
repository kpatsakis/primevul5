    void swap(PValue &other)
    {
      Type_handler_hybrid_field_type::swap(other);
      PValue_simple::swap(other);
      m_decimal.swap(other.m_decimal);
      m_string.swap(other.m_string);
      m_string_ptr.swap(other.m_string_ptr);
    }