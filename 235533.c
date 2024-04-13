    bool read(ReadonlyBytes buffer)
    {
        auto fields_size = sizeof(CentralDirectoryRecord) - (sizeof(u8*) * 3);
        if (buffer.size() < sizeof(central_directory_record_signature) + fields_size)
            return false;
        if (memcmp(buffer.data(), central_directory_record_signature, sizeof(central_directory_record_signature)) != 0)
            return false;
        memcpy(reinterpret_cast<void*>(&made_by_version), buffer.data() + sizeof(central_directory_record_signature), fields_size);
        if (buffer.size() < sizeof(end_of_central_directory_signature) + fields_size + comment_length + name_length + extra_data_length)
            return false;
        name = buffer.data() + sizeof(central_directory_record_signature) + fields_size;
        extra_data = name + name_length;
        comment = extra_data + extra_data_length;
        return true;
    }