  int get_pad_width(const int ngram_width) const {
    // Ngrams can be padded with either a fixed pad width or a dynamic pad
    // width depending on the 'pad_width' arg, but in no case should the padding
    // ever be wider than 'ngram_width' - 1.
    return std::min(pad_width_ < 0 ? ngram_width - 1 : pad_width_,
                    ngram_width - 1);
  }