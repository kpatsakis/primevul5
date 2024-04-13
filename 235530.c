    void write(OutputStream& stream) const
    {
        stream.write_or_error({ local_file_header_signature, sizeof(local_file_header_signature) });
        stream << minimum_version;
        stream << general_purpose_flags;
        stream << compression_method;
        stream << modification_time;
        stream << modification_date;
        stream << crc32;
        stream << compressed_size;
        stream << uncompressed_size;
        stream << name_length;
        stream << extra_data_length;
        if (name_length > 0)
            stream.write_or_error({ name, name_length });
        if (extra_data_length > 0)
            stream.write_or_error({ extra_data, extra_data_length });
        if (compressed_size > 0)
            stream.write_or_error({ compressed_data, compressed_size });
    }