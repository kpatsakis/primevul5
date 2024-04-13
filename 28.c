  bool Connecting() const {
    return ethernet_connecting() || wifi_connecting() || cellular_connecting();
  }
