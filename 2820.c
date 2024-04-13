  void CreateNgrams(const tstring* data, tstring* output, int num_ngrams,
                    int ngram_width) const {
    for (int ngram_index = 0; ngram_index < num_ngrams; ++ngram_index) {
      int pad_width = get_pad_width(ngram_width);
      int left_padding = std::max(0, pad_width - ngram_index);
      int right_padding =
          std::max(0, pad_width - (num_ngrams - (ngram_index + 1)));
      int num_tokens = ngram_width - (left_padding + right_padding);
      int data_start_index = left_padding > 0 ? 0 : ngram_index - pad_width;

      // Calculate the total expected size of the ngram so we can reserve the
      // correct amount of space in the string.
      int ngram_size = 0;
      // Size of the left padding.
      ngram_size += left_padding * left_pad_.length();
      // Size of the tokens.
      for (int n = 0; n < num_tokens; ++n) {
        ngram_size += data[data_start_index + n].length();
      }
      // Size of the right padding.
      ngram_size += right_padding * right_pad_.length();
      // Size of the separators.
      int num_separators = left_padding + right_padding + num_tokens - 1;
      ngram_size += num_separators * separator_.length();

      // Build the ngram.
      tstring* ngram = &output[ngram_index];
      ngram->reserve(ngram_size);
      for (int n = 0; n < left_padding; ++n) {
        ngram->append(left_pad_);
        ngram->append(separator_);
      }
      // Only output first num_tokens - 1 pairs of data and separator
      for (int n = 0; n < num_tokens - 1; ++n) {
        ngram->append(data[data_start_index + n]);
        ngram->append(separator_);
      }
      // Handle case when there are no tokens or no right padding as these can
      // result in consecutive separators.
      if (num_tokens > 0) {
        // If we have tokens, then output last and then pair each separator with
        // the right padding that follows, to ensure ngram ends either with the
        // token or with the right pad.
        ngram->append(data[data_start_index + num_tokens - 1]);
        for (int n = 0; n < right_padding; ++n) {
          ngram->append(separator_);
          ngram->append(right_pad_);
        }
      } else {
        // If we don't have tokens, then the last item inserted into the ngram
        // has been the separator from the left padding loop above. Hence,
        // output right pad and separator and make sure to finish with a
        // padding, not a separator.
        for (int n = 0; n < right_padding - 1; ++n) {
          ngram->append(right_pad_);
          ngram->append(separator_);
        }
        ngram->append(right_pad_);
      }

      // In debug mode only: validate that we've reserved enough space for the
      // ngram.
      DCHECK_EQ(ngram_size, ngram->size());
    }
  }