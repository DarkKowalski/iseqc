#include <ruby.h>

struct iseqc_header
{
    uint32_t mri_version;
    uint32_t page_size;
    uint32_t iseq_start;
    uint8_t  file_crc32;
} __attribute__((packed));

typedef struct iseqc_header iseqc_header_t;
