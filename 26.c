  bool Connected() const {
    return ethernet_connected() || wifi_connected() || cellular_connected();
  }
