  void SetUp() {
    ASSERT_TRUE(data_dir_.CreateUniqueTempDir());
    mock_special_storage_policy_ = new MockSpecialStoragePolicy;
    quota_manager_ = new QuotaManager(
        false /* is_incognito */,
        data_dir_.path(),
        MessageLoopProxy::current(),
        MessageLoopProxy::current(),
        mock_special_storage_policy_);
    quota_manager_->eviction_disabled_ = true;
    additional_callback_count_ = 0;
  }
