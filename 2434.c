  void notify_removers() {
    // Notify all removers. This is because they are
    // waiting for specific keys to appear in the map
    // so we don't know which one to wake up.
    not_empty_.notify_all();
  }