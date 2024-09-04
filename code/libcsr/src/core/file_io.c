////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/core/file_io.h>

#include <sys/stat.h>
#include <unistd.h>
#include <ftw.h>

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
    check_ptr(buffer);

    fio_file *file = fio_open(path, FIO_MODE_READ_ONLY);
    check(file, "could not open the file for reading ... ("string_fmt")", string_fmt_arg(path));

    buffer->byte_length = fio_get_size(file);

    // read data into the buffer
    if (buffer->byte_length > 0)
    {
        buffer->data = calloc(1, buffer->byte_length);
        check_mem(buffer->data);

        size_t bytes_read = fio_read(file, buffer->data, buffer->byte_length);
        check_expr(bytes_read == buffer->byte_length);
    }

    fio_close(file);

    return RC_SUCCESS;

error:

    if (file) {
        fio_close(file);
    }

    if (buffer->data)
    {
        free(buffer->data);

        buffer->data = NULL;
    }

    return RC_FAILURE;
}

result_e fio_save_file(struct string path, struct fio_buffer *buffer)
{
    // cannot write the file if there is a directory with the same name
    check_expr(!fio_fs_is_directory(path));

    // if there is another type of file, make sure it's a regular one (ie. not a symlink)
    if (fio_fs_exists(path)) check_expr(fio_fs_is_file(path));

    check_ptr(buffer);
    check_expr(buffer->data != NULL && buffer->byte_length > 0);

    // FIXME

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

fio_file* fio_open(struct string path, enum fio_mode mode)
{
    check_expr(fio_fs_is_file(path));

    fio_file *file = calloc(1, sizeof(struct fio_file));
    check_mem(file);

    ////////////////////////////////////////

    string_cstr mode_cstr = NULL;

    switch (mode)
    {
        case FIO_MODE_WRITE_ONLY:
            mode_cstr = "w";
            break;

        case FIO_MODE_READ_WRITE:
            mode_cstr = "w+";
            break;
        
        default: {
            mode_cstr = "r";
        }
    }

    file->stream = fopen(_cstr_from_string(path), mode_cstr);
    check(file->stream, "could not open file : "string_fmt, string_fmt_arg(path));

    struct stat buffer = {0};
    check_expr(fstat(fileno(file->stream), &buffer) == 0);

    file->size = buffer.st_size;

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
    check_ptr(file->stream);
    check_expr(file->size > 0 && file->size > position);

    // FIXME

error:
    return;
}

void fio_seek_by(fio_file *file, u32 count)
{
    check_ptr(file);
    check_ptr(file->stream);

    check_expr(count > 0);

    // FIXME

error:
    return;
}

u32 fio_tell(fio_file *file)
{
    check_ptr(file);
    check_ptr(file->stream);

    // FIXME

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

struct string fio_fs_normalize_path(struct string path, struct arena *arena)
{
    check_quiet(string_is_valid(path));

    // holy moly, strip a trailing slash from path
    if (path.length > 1 && path.ptr[path.length - 1] == '/') {
        path.length--;
    }

    // FIXME need arena impl.

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

struct string fio_fs_get_current_path(struct arena *arena)
{
    // check_ptr(arena);

    string_cstr cwd = getcwd(NULL, 0);
    check_ptr(cwd);

    // FIXME need arena impl.

    return make_string_from_cstr(cwd);

error:
    return make_string("");
}

struct string fio_fs_get_parent_path(struct string path)
{
    check_expr(string_is_valid(path));

    return string_cut(path, string_rfind(path, '/')).left;

error:
    return path;
}

struct string fio_fs_get_absolute_path(struct string path, struct arena *arena)
{
    check_expr(string_is_valid(path));
    // check_ptr(arena);

    string_cstr real_path = realpath(_cstr_from_string(path), NULL);
    check_ptr(real_path);

    // FIXME need arena impl.

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

    return string_cut(path, string_rfind(path, '/')).right;

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

static enum fio_file_type _get_file_type(struct string path)
{
    check_expr(string_is_valid(path));

    struct stat buffer = {0};
    check_quiet(lstat(_cstr_from_string(path), &buffer)== 0);

    // for now only regular files and directories are needed
    if (S_ISREG(buffer.st_mode)) return FIO_FILE_TYPE_REGULAR_FILE;
    if (S_ISDIR(buffer.st_mode)) return FIO_FILE_TYPE_DIRECTORY;

    return FIO_FILE_TYPE_OTHER;

error:
    return FIO_FILE_TYPE_NONE;
}

bool fio_fs_exists(struct string path)
{
    return _get_file_type(path) != FIO_FILE_TYPE_NONE;
}

bool fio_fs_is_file(struct string path)
{
    return _get_file_type(path) == FIO_FILE_TYPE_REGULAR_FILE;
}

bool fio_fs_is_directory(struct string path)
{
    return _get_file_type(path) == FIO_FILE_TYPE_DIRECTORY;
}

bool fio_fs_file_has_extension(struct string path, struct string ext)
{
    return string_equals(fio_fs_get_file_extension(path), ext);
}

result_e fio_fs_create_directory(struct string path)
{
    check_expr(!fio_fs_exists(path));

    s32 position = -1;

    do {
        position = string_find_at(path, position + 1, '/');

        struct string my_path = string_substr(path, 0, position + 1);
        my_path = fio_fs_normalize_path(my_path, NULL);

        // no file conflict
        //   - create the directory and continue the loop :)
        if (!fio_fs_exists(my_path))
        {
            clog_trace("creating (directory) "string_fmt" ...", string_fmt_arg(my_path));

            // file perms: rwx for owner and group, rx for others
            s32 result = mkdir(_cstr_from_string(my_path), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
            check(result == 0, "could not create directory : "string_fmt, string_fmt_arg(my_path));

            continue;
        }

        // file conflict
        //   - if it's a directory, just skip it
        //   - if it's a file of some kind (regular, symlink, ...), abort XXX
        if (!fio_fs_is_directory(my_path))
        {
            clog_error("file conflict, aborting foo ... (" string_fmt ")", string_fmt_arg(my_path));
            break;
        }

    } while (position != -1);

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static s32 _on_remove_element(string_cstr path, const struct stat *sb, s32 typeflag, struct FTW *ftwbuf)
{
    clog_trace("removing (%s) %s ...", (S_ISDIR(sb->st_mode)) ? "directory" : "file", path);

    return remove(path);
}

result_e fio_fs_remove(struct string path)
{
    check_expr(fio_fs_exists(path));

    s32 result = nftw(_cstr_from_string(path), _on_remove_element, 64, FTW_DEPTH | FTW_PHYS);
    check(result == 0, "could not remove file or directory ... ("string_fmt")", string_fmt_arg(path));

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}
