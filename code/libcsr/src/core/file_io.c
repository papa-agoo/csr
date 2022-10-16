////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/core/file_io.h>

#include <aio.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct fio_file
{
    FILE *fp;
    size_t size;
};

result_e fio_read_file(const char *uri, struct fio_buffer *buffer)
{
    check_ptr(uri);
    check_ptr(buffer);

    fio_file *file = fio_open(uri, FIO_MODE_READ_ONLY);
    check_ptr(file);

    buffer->byte_length = fio_get_size(file);

    buffer->data = calloc(1, buffer->byte_length);
    check_mem(buffer->data);

    size_t bytes_read = fio_read(file, buffer->data, buffer->byte_length);
    check_expr(bytes_read == buffer->byte_length);

    fio_close(file);

    return RC_SUCCESS;

error:

    if (file) {
        fio_close(file);
    }

    if (buffer->data) {
        free(buffer->data);
    }

    return RC_FAILURE;
}

result_e fio_read_line(fio_file *file, struct fio_buffer *buffer)
{
    check_ptr(file);
    check_ptr(file->fp);

    check_ptr(buffer);

    ssize_t bytes_read = getline((char**)&buffer->data, &buffer->byte_length, file->fp);

    return (bytes_read == -1) ? RC_FAILURE : RC_SUCCESS;

error:
    return RC_FAILURE;
}

fio_file* fio_open(const char *uri, enum fio_mode mode)
{
    check_ptr(uri);

    ////////////////////////////////////////

    // FIXME ugly stuff
    const char *mode_ro = "r";
    const char *mode_wo = "w";
    const char *mode_rw = "w+";

    const char *mode_str = NULL;

    switch (mode)
    {
        case FIO_MODE_READ_ONLY:
            mode_str = mode_ro;
            break;

        case FIO_MODE_WRITE_ONLY:
            mode_str = mode_wo;
            break;

        case FIO_MODE_READ_WRITE:
            mode_str = mode_rw;
            break;
        
        default: {
            clog_trace("fio_mode unknown, falling back to FIO_MODE_READ_ONLY");
            mode_str = mode_ro;
        }
    }

    ////////////////////////////////////////

    fio_file *file = calloc(1, sizeof(struct fio_file));
    check_mem(file);

    file->fp = fopen(uri, mode_str);
    check(file->fp, "could not open file : %s", uri);

    // FIXME
    // https://wiki.sei.cmu.edu/confluence/display/c/FIO19-C.+Do+not+use+fseek%28%29+and+ftell%28%29+to+compute+the+size+of+a+regular+file

    // get file size
    fseek(file->fp, 0, SEEK_END);
    file->size = ftell(file->fp);
    fseek(file->fp, 0, SEEK_SET);

    ////////////////////////////////////////

    return file;

error:
    if (file) {
        free(file);
    }

    return NULL;
}

void fio_close(fio_file *file)
{
    check_ptr(file);
    check_ptr(file->fp);

    fclose(file->fp);

    free(file);

error:
    return;
}

size_t fio_get_size(fio_file *file)
{
    check_ptr(file);

    return file->size;

error:
    return 0;
}

FILE* fio_get_fp(fio_file *file)
{
    check_ptr(file);

    return file->fp;

error:
    return NULL;
}

size_t fio_read(fio_file *file, void *buffer, size_t byte_length)
{
    check_ptr(file);
    check_ptr(file->fp);

    check_ptr(buffer);
    check_expr(byte_length > 0);

    return fread(buffer, sizeof(u8), byte_length, file->fp);

error:
    return 0;
}

size_t fio_write(fio_file *file, void *buffer, size_t byte_length)
{
    check_ptr(file);
    check_ptr(file->fp);

    check_ptr(buffer);
    check_expr(byte_length > 0);

    return fwrite(buffer, sizeof(u8), byte_length, file->fp);

error:
    return 0;
}

void fio_seek_to(fio_file *file, s32 position)
{
    check_ptr(file);

    // rewind(file->fp);
    // fseek(file->fp, )

error:
    return;
}

void fio_seek_by(fio_file *file, u32 count)
{
    check_ptr(file);
    check_expr(count > 0);

error:
    return;
}

u32 fio_tell(fio_file *file)
{
    check_ptr(file);

error:
    return 0;
}
