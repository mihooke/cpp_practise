#include "Buffer.h"

#include <errno.h>
#include <sys/uio.h>

namespace mihooke {

const char *Buffer::kCRLF = "\r\n";

ssize_t Buffer::readFromFd(int fd, int *errorNo) {
  char buf[65536];
  struct iovec iv[2];
  const size_t originWrite = writableBytes();
  iv[0].iov_base = begin() + _writeIndex;
  iv[0].iov_len = originWrite;
  iv[1].iov_base = buf;
  iv[1].iov_len = sizeof(buf);
  const int iovcnt = (originWrite < sizeof(buf)) ? 2 : 1;
  const ssize_t n = ::readv(fd, iv, iovcnt);
  if (n < 0) {
    *errorNo = errno;
  } else if (static_cast<size_t>(n) <= originWrite) {
    _writeIndex += n;
  } else {
    _writeIndex = _data.size();
    append(buf, n - originWrite);
  }
  return n;
}

}  // namespace mihooke
