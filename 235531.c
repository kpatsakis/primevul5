    [[nodiscard]] size_t size() const
    {
        return sizeof(central_directory_record_signature) + (sizeof(CentralDirectoryRecord) - (sizeof(u8*) * 3)) + name_length + extra_data_length + comment_length;
    }