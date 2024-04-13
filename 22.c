  virtual void ConnectToWifiNetwork(const WifiNetwork* network,
                                    const std::string& password,
                                    const std::string& identity,
                                    const std::string& certpath) {
    DCHECK(network);
    if (!EnsureCrosLoaded())
      return;
    if (ConnectToNetworkWithCertInfo(network->service_path().c_str(),
        password.empty() ? NULL : password.c_str(),
        identity.empty() ? NULL : identity.c_str(),
        certpath.empty() ? NULL : certpath.c_str())) {
      WifiNetwork* wifi = GetWirelessNetworkByPath(
          wifi_networks_, network->service_path());
      if (wifi) {
        wifi->set_passphrase(password);
        wifi->set_identity(identity);
        wifi->set_cert_path(certpath);
        wifi->set_connecting(true);
        wifi_ = wifi;
      }
      NotifyNetworkManagerChanged();
    }
  }
