////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/base.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct fio_file fio_file;

enum fio_mode
{
    FIO_MODE_READ_ONLY,
    FIO_MODE_WRITE_ONLY,
    FIO_MODE_READ_WRITE,
};

struct fio_buffer
{
    void *data;
    size_t byte_length;
};

////////////////////////////////////////////////////////////

// load the contents of a file to a buffer
result_e fio_load_file(struct string path, struct fio_buffer *buffer);

// load the contents of a buffer to a file
result_e fio_save_file(struct string path, struct fio_buffer *buffer);


// file stream api
fio_file* fio_open(struct string path, enum fio_mode mode);
void fio_close(fio_file *file);

size_t fio_read(fio_file *file, void *buffer, size_t byte_length);
size_t fio_write(fio_file *file, void *buffer, size_t byte_length);

// void fio_seek_to(fio_file *file, s32 position);
// void fio_seek_by(fio_file *file, u32 count);
// u32 fio_tell(fio_file *file);

size_t fio_get_size(fio_file *file);
FILE* fio_get_stream(fio_file *file);

// result_e fio_read_line(fio_file *file, struct fio_buffer *buffer);

////////////////////////////////////////////////////////////

// try to normalize an ugly path (ie. //foo/bar/./baz/ -> /foo/bar/baz)
struct string fio_fs_normalize_path(struct string path);


// check if a path is valid (ie. {APP_CONFIG_DIR}/application.ini is not a valid path)
bool fio_fs_is_valid_path(struct string path);

// check if a path is absolute (ie. /foo/bar)
bool fio_fs_is_absolute_path(struct string path);

// check if a path is relative (ie. ../foo/bar)
bool fio_fs_is_relative_path(struct string path);


// current working directory
struct string fio_fs_get_current_path();

// get the full path to the parent (normally a directory)
struct string fio_fs_get_parent_path(struct string path);

// get the full path to the relative path
struct string fio_fs_get_absolute_path(struct string relative_path);

// get path relative to current path (working directory)
struct string fio_fs_get_relative_path(struct string absolute_path);

// get the file name from the path
struct string fio_fs_get_file_name(struct string path);

// get the file extension from the path
struct string fio_fs_get_file_extension(struct string path);


// check if the path points to a valid file type (regular file, directory or other file types like a symlink)
bool fio_fs_exists(struct string path);

// check if the path points to a regular file
bool fio_fs_is_file(struct string path);

// check if the path points to a directory
bool fio_fs_is_directory(struct string path);

// check if the path has a given extension (ie. "txt")
bool fio_fs_file_has_extension(struct string path, struct string ext);

////////////////////////////////////////////////////////////

// enum fio_file_type
// {
//     FIO_FILE_TYPE_UNKNOWN,

//     FIO_FILE_TYPE_REGULAR_FILE,
//     FIO_FILE_TYPE_DIRECTORY,
// };

// result_e fio_fs_create_file(struct string path);
// result_e fio_fs_create_directory(struct string path);
// result_e fio_fs_copy(struct string src, struct string dst);
// result_e fio_fs_rename(struct string src, struct string dst);
// result_e fio_fs_remove(struct string path);

// void fio_fs_list_directory(struct string path);
// void fio_fs_traverse_directory_tree(struct string path);

////////////////////////////////////////////////////////////
