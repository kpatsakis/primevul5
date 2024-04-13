  virtual void ConnectToWifiNetwork(ConnectionSecurity security,
                                    const std::string& ssid,
                                    const std::string& password,
                                    const std::string& identity,
                                    const std::string& certpath,
                                    bool auto_connect) {
    if (!EnsureCrosLoaded())
      return;

    ServiceInfo* service = GetWifiService(ssid.c_str(), security);
    if (service) {
      SetAutoConnect(service->service_path, auto_connect);
      ConnectToNetworkWithCertInfo(service->service_path,
          password.empty() ? NULL : password.c_str(),
          identity.empty() ? NULL : identity.c_str(),
          certpath.empty() ? NULL : certpath.c_str());

      FreeServiceInfo(service);
    } else {
      LOG(WARNING) << "Cannot find hidden network: " << ssid;
    }
  }
