////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/core/file_io.h>

#include <aio.h>
#include <sys/stat.h>

#include <unistd.h>
#include <stdlib.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

// FIXME this is needed until cstr_from_string() is impl.
#define STR_BUF_SIZE 256
static u8 g_str_buf[STR_BUF_SIZE];

static inline string_cstr _cstr_from_string(struct string str)
{
    snprintf(g_str_buf, STR_BUF_SIZE, string_fmt, string_fmt_arg(str));

    return g_str_buf;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

struct fio_file
{
    FILE *stream;
    size_t size;
};

result_e fio_load_file(struct string path, struct fio_buffer *buffer)
{
    check_expr(string_is_valid(path));
    check_ptr(buffer);

    fio_file *file = fio_open(path, FIO_MODE_READ_ONLY);
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

result_e fio_save_file(struct string path, struct fio_buffer *buffer)
{
error:
    return RC_FAILURE;
}

fio_file* fio_open(struct string path, enum fio_mode mode)
{
    check_expr(string_is_valid(path));

    ////////////////////////////////////////

    // FIXME ugly stuff
    string_cstr mode_ro = "r";
    string_cstr mode_wo = "w";
    string_cstr mode_rw = "w+";

    string_cstr mode_str = NULL;

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

    string_cstr path_cstr = _cstr_from_string(path);

    file->stream = fopen(path_cstr, mode_str);
    check(file->stream, "could not open file : %s", path_cstr);

    // FIXME

    // get file size
    fseek(file->stream, 0, SEEK_END);
    file->size = ftell(file->stream);
    fseek(file->stream, 0, SEEK_SET);

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
    check_ptr(file->stream);

    fclose(file->stream);

    free(file);

error:
    return;
}

size_t fio_read(fio_file *file, void *buffer, size_t byte_length)
{
    check_ptr(file);
    check_ptr(file->stream);

    check_ptr(buffer);
    check_expr(byte_length > 0);

    return fread(buffer, sizeof(u8), byte_length, file->stream);

error:
    return 0;
}

size_t fio_write(fio_file *file, void *buffer, size_t byte_length)
{
    check_ptr(file);
    check_ptr(file->stream);

    check_ptr(buffer);
    check_expr(byte_length > 0);

    return fwrite(buffer, sizeof(u8), byte_length, file->stream);

error:
    return 0;
}

void fio_seek_to(fio_file *file, s32 position)
{
    check_ptr(file);

    // rewind(file->stream);
    // fseek(file->stream, )

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

size_t fio_get_size(fio_file *file)
{
    check_ptr(file);

    return file->size;

error:
    return 0;
}

FILE* fio_get_stream(fio_file *file)
{
    check_ptr(file);

    return file->stream;

error:
    return NULL;
}

result_e fio_read_line(fio_file *file, struct fio_buffer *buffer)
{
    check_ptr(file);
    check_ptr(file->stream);

    check_ptr(buffer);

    ssize_t bytes_read = getline((char**)&buffer->data, &buffer->byte_length, file->stream);

    return (bytes_read == -1) ? RC_FAILURE : RC_SUCCESS;

error:
    return RC_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

struct string fio_fs_normalize_path(struct string path)
{
    check_quiet(string_is_valid(path));

    clog_warn("not impl. yet");

error:
    return path;
}

bool fio_fs_is_valid_path(struct string path)
{
    // the single most important path test :)
    return (string_find(path, '\0') == -1) ? true : false;
}

bool fio_fs_is_absolute_path(struct string path)
{
    check_expr(string_is_valid(path));

    return string_starts_with(path, make_string("/"));

error:
    return false;
}

bool fio_fs_is_relative_path(struct string path)
{
    return !fio_fs_is_absolute_path(path);
}

struct string fio_fs_get_current_path()
{
    string_cstr cwd = getcwd(NULL, 0);
    check_ptr(cwd);

    clog_warn("FIXME: free(cwd)");

    return make_string_from_cstr(cwd);

error:
    return make_string("");
}

struct string fio_fs_get_parent_path(struct string path)
{
    check_expr(string_is_valid(path));

    clog_warn("fix string_substr()");

    return string_rchop(path, string_rfind(path, '/') + 1);

error:
    return path;
}

struct string fio_fs_get_absolute_path(struct string path)
{
    check_expr(string_is_valid(path));

    string_cstr real_path = realpath(_cstr_from_string(path), NULL);
    check_ptr(real_path);

    clog_warn("FIXME: free(real_path)");

    return make_string_from_cstr(real_path);

error:
    return path;
}

struct string fio_fs_get_relative_path(struct string path)
{
    check_expr(string_is_valid(path));

    clog_warn("not impl. yet");

    // [2, 1] {/opt/csr/},{bin}
    // [2, 3] {/opt/csr/},{resources/fonts/Consolas.ttf}

error:
    return path;
}

struct string fio_fs_get_file_name(struct string path)
{
    check_expr(string_is_valid(path));

    return string_chop(path, string_rfind(path, '/') + 1);

error:
    return make_string("");
}

struct string fio_fs_get_file_extension(struct string path)
{
    // extract filename from the path
    struct string filename = fio_fs_get_file_name(path);
    check_expr(string_is_valid(filename));

    // convert a dotfile to a regular file (ie. .sectret.txt -> secret.txt)
    if (string_starts_with(filename, make_string("."))) {
        filename = string_chop(filename, 1);
    }

    // if there is still a dot in the filename, extract the extension
    s32 delimiter_idx = string_rfind(filename, '.');

    if (delimiter_idx >= 0) {
        return string_chop(filename, delimiter_idx + 1);
    }

error:
    return make_string("");
}


bool fio_fs_exists(struct string path)
{
    check_expr(string_is_valid(path));

    clog_warn("not impl. yet");

error:
    return false;
}

bool fio_fs_is_file(struct string path)
{
    check_expr(fio_fs_exists(path));

    clog_warn("not impl. yet");

error:
    return false;
}

bool fio_fs_is_directory(struct string path)
{
    check_expr(fio_fs_exists(path));

    clog_warn("not impl. yet");

error:
    return false;
}

bool fio_fs_file_has_extension(struct string path, struct string ext)
{
    return string_equals(fio_fs_get_file_extension(path), ext);
}
