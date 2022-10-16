////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/base.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

enum fio_mode
{
    FIO_MODE_READ_ONLY,
    FIO_MODE_WRITE_ONLY,
    FIO_MODE_READ_WRITE,
};

typedef struct fio_file fio_file;

struct fio_buffer
{
    void *data;
    size_t byte_length;
};

result_e fio_read_file(const char *uri, struct fio_buffer *buffer);
result_e fio_read_line(fio_file *file, struct fio_buffer *buffer);

fio_file* fio_open(const char *uri, enum fio_mode mode);
void fio_close(fio_file *file);

size_t fio_get_size(fio_file *file);
FILE* fio_get_fp(fio_file *file);

size_t fio_read(fio_file *file, void *buffer, size_t byte_length);
size_t fio_write(fio_file *file, void *buffer, size_t byte_length);

void fio_seek_to(fio_file *file, s32 position); // FIXME
void fio_seek_by(fio_file *file, u32 count); // FIXME
u32 fio_tell(fio_file *file); // FIXME
