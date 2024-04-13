  MockStorageClient* CreateClient(
      const MockOriginData* mock_data, size_t mock_data_size) {
    return new MockStorageClient(quota_manager_->proxy(),
                                 mock_data, mock_data_size);
  }
