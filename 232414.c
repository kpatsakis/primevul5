static inline bool hid_is_using_ll_driver(struct hid_device *hdev,
		struct hid_ll_driver *driver)
{
	return hdev->ll_driver == driver;
}