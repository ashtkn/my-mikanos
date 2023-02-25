#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

void _exit(void) {
  while (1) {
    __asm__("hlt");  // NOLINT(hicpp-no-assembler)
  }
}

caddr_t program_break, program_break_end;

caddr_t sbrk(int incr) {
  if (program_break == 0 || program_break + incr >= program_break_end) {
    errno = ENOMEM;  // Out of memory
    return (caddr_t)-1;
  }
  caddr_t prev_break = program_break;
  program_break += incr;
  return prev_break;
}

int getpid(void) { return 1; }

// NOLINTNEXTLINE(misc-unused-parameters)
int kill(int pid, int sig) {
  errno = EINVAL;  // Invalid argument
  return -1;
}

// NOLINTNEXTLINE(misc-unused-parameters)
int close(int fd) {
  errno = EBADF;  // Bad file number
  return -1;
}

// NOLINTNEXTLINE(misc-unused-parameters)
off_t lseek(int fd, off_t offset, int whence) {
  errno = EBADF;  // Bad file number
  return -1;
}

// NOLINTNEXTLINE(misc-unused-parameters)
int open(const char* path, int flags) {
  errno = ENOENT;  // No such file or directory
  return -1;
}

// NOLINTNEXTLINE(misc-unused-parameters)
ssize_t read(int fd, void* buf, size_t count) {
  errno = EBADF;  // Bad file number
  return -1;
}

// NOLINTNEXTLINE(misc-unused-parameters)
ssize_t write(int fd, const void* buf, size_t count) {
  errno = EBADF;  // Bad file number
  return -1;
}

// NOLINTNEXTLINE(misc-unused-parameters,readability-inconsistent-declaration-parameter-name)
int fstat(int fd, struct stat* buf) {
  errno = EBADF;  // Bad file number
  return -1;
}

// NOLINTNEXTLINE(misc-unused-parameters)
int isatty(int fd) {
  errno = EBADF;  // Bad file number
  return -1;
}
