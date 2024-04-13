  explicit StagingMap(const DataTypeVector& dtypes, std::size_t capacity,
                      std::size_t memory_limit)
      : dtypes_(dtypes),
        capacity_(capacity),
        memory_limit_(memory_limit),
        current_bytes_(0) {}