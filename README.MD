# John_Wood_s_KVDB_Project_C_CPP

## DB File internal layout

### [DBFileHeader]
```
struct DBFileHeader {
    ubyte_t     Magic[MAGIC_SIZE];  // 8 bytes; { 'K', 'V', 'D', 'B', '\r', '\n', ' ', '\0' };
    uint16_t    Version;            // 2 bytes; 5 I guess
    ubyte_t     ByteOrder;          // 1 byte ; LE only; BE is NOT Supported
    size32_t    HeaderSize;         // 4 bytes; (uint32_t)sizeof(struct DBFileHeader)
    size32_t    IndexEntrySize;     // 4 bytes; (uint32_t)sizeof(struct DBIndexEntry)
    ulong_t     EntryCapacity;      // 4 bytes; Best practice: set it to 4096 since this db is not resizable
    ulong_t     EntryCount;         // 4 bytes; how many entries you have already inserted (including deleted ones)
    ulong_t     ValidEntryCount;    // 4 bytes; EntryCount - DeletedEntryCount; deletion is only logical
    size32_t    DataEntryHeaderSize;// 4 bytes; (uint32_t)sizeof(DataEntryHeader)
    off64_t     IndexTableOffset;   // 8 bytes; located after all data entries; before EOF
    off64_t     DataSectionOffset;  // 8 bytes; after DBFileHeader
    off64_t     EOFHeaderOffset;    // 8 bytes; FileEOF - sizeof(DBEOFMarker)
    qword_t     LastModified;       // 8 bytes; unix time; haven't inplemented yet
    ubyte_t     Reserved[5];        // 5 bytes of padding, pad struct to 8 byte aligned
};
```

### [DBIndexEntry]
(IndexTableEntry)

```
struct DBIndexEntry {
    hash_t      KeyHash;    // 8 bytes; FNV-1a hash
    ulong_t     EntryID;    // 4 bytes; Index of Record
    ulong_t     Flags;      // 4 bytes; FLAG_UNUSED, FLAG_VALID, FLAG_DELETED
    size32_t    Size;       // 4 bytes; sizeof Record[EntryID] in [DataSection]
    off64_t     Offset;     // 8 bytes; offset of record in [DataSection]
    ulong_t     Reserved;   // pad to 8 byte aligned
};
```
