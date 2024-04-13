    bool read(ReadonlyBytes buffer)
    {
        auto fields_size = sizeof(LocalFileHeader) - (sizeof(u8*) * 3);
        if (buffer.size() < sizeof(local_file_header_signature) + fields_size)
            return false;
        if (memcmp(buffer.data(), local_file_header_signature, sizeof(local_file_header_signature)) != 0)
            return false;
        memcpy(reinterpret_cast<void*>(&minimum_version), buffer.data() + sizeof(local_file_header_signature), fields_size);
        if (buffer.size() < sizeof(end_of_central_directory_signature) + fields_size + name_length + extra_data_length + compressed_size)
            return false;
        name = buffer.data() + sizeof(local_file_header_signature) + fields_size;
        extra_data = name + name_length;
        compressed_data = extra_data + extra_data_length;
        return true;
    }