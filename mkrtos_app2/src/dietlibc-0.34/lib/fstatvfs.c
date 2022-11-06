#define _FILE_OFFSET_BITS 64
#include <sys/statvfs.h>
#include <sys/statfs.h>

extern void __statvfs_cvt(struct statfs* from,struct statvfs* to);

int fstatvfs(int fd, struct statvfs *sv) {
  struct statfs ss;
  if (fstatfs(fd,&ss)==-1) return -1;
  __statvfs_cvt(&ss,sv);
  return 0;
}
