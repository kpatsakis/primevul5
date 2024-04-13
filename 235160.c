    bool needs_conversion() const
    {
      return final_character_set_of_str_value !=
             character_set_of_placeholder;
    }