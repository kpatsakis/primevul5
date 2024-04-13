void QuotaManagerTest::GetUsage_WithModifyTestBody(const StorageType type) {
  const MockOriginData data[] = {
    { "http://foo.com/",   type,  10 },
    { "http://foo.com:1/", type,  20 },
  };
  MockStorageClient* client = CreateClient(data, ARRAYSIZE_UNSAFE(data));
  RegisterClient(client);

  GetUsageAndQuota(GURL("http://foo.com/"), type);
  MessageLoop::current()->RunAllPending();
  EXPECT_EQ(kQuotaStatusOk, status());
  EXPECT_EQ(10 + 20, usage());

  client->ModifyOriginAndNotify(GURL("http://foo.com/"), type, 30);
  client->ModifyOriginAndNotify(GURL("http://foo.com:1/"), type, -5);
  client->AddOriginAndNotify(GURL("https://foo.com/"), type, 1);

  GetUsageAndQuota(GURL("http://foo.com/"), type);
  MessageLoop::current()->RunAllPending();
  EXPECT_EQ(kQuotaStatusOk, status());
  EXPECT_EQ(10 + 20 + 30 - 5 + 1, usage());
  int foo_usage = usage();

  client->AddOriginAndNotify(GURL("http://bar.com/"), type, 40);
  GetUsageAndQuota(GURL("http://bar.com/"), type);
  MessageLoop::current()->RunAllPending();
  EXPECT_EQ(kQuotaStatusOk, status());
  EXPECT_EQ(40, usage());

  GetGlobalUsage(type);
  MessageLoop::current()->RunAllPending();
  EXPECT_EQ(foo_usage + 40, usage());
  EXPECT_EQ(0, unlimited_usage());
}
