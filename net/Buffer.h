#ifndef MIHOOKE_BUFFER_H
#define MIHOOKE_BUFFER_H

#include <endian.h>
#include <string.h>
#include <algorithm>
#include <string>
#include <vector>

namespace mihooke {
class Buffer {
 public:
  static const size_t kInitialSize{1024};
  static const size_t kPrependSize{8};
  explicit Buffer(size_t size = kInitialSize)
      : _data(kPrependSize + kInitialSize),
        _readIndex(kPrependSize),
        _writeIndex(kPrependSize) {}

  void append(const char *data, size_t len) {
    if (writableBytes() < len) {
      //检查剩余空间以及可读区域的大小之和是否容得下data：容不下则resize，否则可读区域数据往前挪腾出空间
      if (writableBytes() + prependableBytes() < len + kPrependSize) {
        _data.resize(_writeIndex + len);
      } else {
        size_t originReadSize = readableBytes();
        std::copy(begin() + _readIndex, begin() + _writeIndex,
                  begin() + kPrependSize);
        _readIndex = kPrependSize;
        _writeIndex = originReadSize + _readIndex;
      }
    }
    std::copy(data, data + len, begin() + _writeIndex);
    _writeIndex += len;
  }
  void append(const void *data, size_t len) {
    append(static_cast<const char *>(data), len);
  }
  void append(const std::string &data) { append(data.c_str(), data.length()); }
  void appendInt8(int8_t v) { append(&v, sizeof(v)); }
  void appendInt16(int16_t v) {
    int16_t val = htobe16(v);
    append(&val, sizeof(int16_t));
  }
  void appendInt32(int32_t v) {
    int32_t val = htobe32(v);
    append(&val, sizeof(int32_t));
  }
  void appendInt64(int64_t v) {
    int64_t val = htobe64(v);
    append(&val, sizeof(int64_t));
  }
  int8_t peekInt8() const { return *peek(); }
  int16_t peekInt16() const {
    int16_t v{0};
    memcpy(&v, peek(), sizeof(int16_t));
    return be16toh(v);
  }
  int32_t peekInt32() const {
    int32_t v{0};
    memcpy(&v, peek(), sizeof(int32_t));
    return be32toh(v);
  }
  int64_t peekInt64() const {
    int64_t v{0};
    memcpy(&v, peek(), sizeof(int64_t));
    return be64toh(v);
  }
  int8_t readInt8() {
    int8_t v = peekInt8();
    retrieveInt8();
    return v;
  }
  int16_t readInt16() {
    int16_t v = peekInt16();
    retrieveInt16();
    return v;
  }
  int32_t readInt32() {
    int32_t v = peekInt32();
    retrieveInt32();
    return v;
  }
  int64_t readInt64() {
    int64_t v = peekInt64();
    retrieveInt64();
    return v;
  }
  void prepend(const void *data, size_t len) {
    _readIndex -= len;
    const char *d = static_cast<const char *>(data);
    std::copy(d, d + len, begin() + _readIndex);
  }
  void prependInt8(int8_t v) { prepend(&v, sizeof(int8_t)); }
  void prependInt16(int16_t v) {
    int16_t vp = htobe16(v);
    prepend(&vp, sizeof(int16_t));
  }
  void prependInt32(int32_t v) {
    int32_t vp = htobe32(v);
    prepend(&vp, sizeof(int32_t));
  }
  void prependInt64(int64_t v) {
    int64_t vp = htobe64(v);
    prepend(&vp, sizeof(int64_t));
  }

  int prependableBytes() const { return _readIndex; }
  int readableBytes() const { return _writeIndex - _readIndex; }
  int writableBytes() const { return _data.size() - _writeIndex; }

  const char *peek() const { return begin() + _readIndex; }
  const char *findCRLF() const {
    const char *result =
        std::search(peek(), begin() + _writeIndex, kCRLF, kCRLF + 2);
    return result == begin() + _writeIndex ? nullptr : result;
  }
  const char *findEOL() const {
    const void *result = memchr(peek(), '\n', readableBytes());
    return static_cast<const char *>(result);
  }

  void retrieve(size_t len) {
    if (len < readableBytes()) {
      _readIndex += len;
    } else {
      retrieveAll();
    }
  }
  void retrieveAll() { _readIndex = _writeIndex = kPrependSize; }
  void retrieveUntil(const char *end) { retrieve(end - peek()); }
  void retrieveInt8() { retrieve(sizeof(int8_t)); }
  void retrieveInt16() { retrieve(sizeof(int16_t)); }
  void retrieveInt32() { retrieve(sizeof(int32_t)); }
  void retrieveInt64() { retrieve(sizeof(int64_t)); }
  std::string retrieveAllAsString() {
    return retrieveAsString(readableBytes());
  }
  std::string retrieveAsString(size_t len) {
    std::string result{peek(), len};
    retrieve(len);
    return result;
  }
  std::string retrieveAsString(const char *end) {
    std::string result{peek(), static_cast<size_t>(end - peek())};
    retrieve(end - peek());
    return result;
  }

  // void shrink() { _data.shrink_to_fit(); }
  ssize_t readFromFd(int fd, int *errorNo);

 private:
  char *begin() { return &*(_data.begin()); }
  const char *begin() const { return &*(_data.begin()); }

 private:
  std::vector<char> _data;
  int _readIndex;
  int _writeIndex;

  static const char *kCRLF;
};

}  // namespace mihooke

#endif