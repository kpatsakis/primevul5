  int get_num_ngrams(const int length, const int ngram_width) const {
    int pad_width = get_pad_width(ngram_width);
    return std::max(0, ((length + 2 * pad_width) - ngram_width) + 1);
  }