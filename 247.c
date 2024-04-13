  void DumpOriginInfoTable() {
    origin_info_entries_.clear();
    quota_manager_->DumpOriginInfoTable(
        callback_factory_.NewCallback(
             &QuotaManagerTest::DidDumpOriginInfoTable));
   }
