#include "../net/Buffer.h"
#include <assert.h>
#include "../base/Logging.h"

using namespace mihooke;

int main() {
  {
    Buffer buf1;
    assert(buf1.writableBytes() == Buffer::kInitialSize);
    assert(buf1.readableBytes() == 0);
    assert(buf1.prependableBytes() == Buffer::kPrependSize);

    Buffer buf2;
    const std::string temp(200, '0');
    buf2.append(temp);
    assert(buf2.writableBytes() == Buffer::kInitialSize - 200);
    assert(buf2.readableBytes() == 200);
    assert(buf2.prependableBytes() == Buffer::kPrependSize);
    const std::string r_str2 = buf2.retrieveAsString(60);
    assert(r_str2.size() == 60);
    assert(buf2.readableBytes() == 140);
    assert(buf2.writableBytes() == Buffer::kInitialSize - 200);
    assert(buf2.prependableBytes() == Buffer::kPrependSize + r_str2.size());

    buf2.append(std::string(200, '0'));
    assert(buf2.writableBytes() == Buffer::kInitialSize - 400);
    assert(buf2.readableBytes() == 340);
    assert(buf2.prependableBytes() == Buffer::kPrependSize + r_str2.size());
    const std::string r_str3 = buf2.retrieveAllAsString();
    assert(r_str3.size() == 340);
    assert(buf2.readableBytes() == 0);
    assert(buf2.writableBytes() == Buffer::kInitialSize);
    assert(buf2.prependableBytes() == Buffer::kPrependSize);
  }
  {
    Buffer buf3;
    buf3.append(std::string(400, '0'));
    assert(buf3.readableBytes() == 400);
    assert(buf3.writableBytes() == Buffer::kInitialSize - 400);

    buf3.append(std::string(1400, '1'));
    assert(buf3.readableBytes() == 1800);
    assert(buf3.writableBytes() == 0);  // 扩容1800，导致_writeIndex和size()一样

    buf3.retrieveAll();
    assert(buf3.readableBytes() == 0);
    assert(buf3.writableBytes() == 1800);
    assert(buf3.prependableBytes() == 8);

    Buffer buf4;
    buf4.append(std::string(800, '2'));
    assert(buf4.readableBytes() == 800);
    assert(buf4.writableBytes() == Buffer::kInitialSize - 800);

    buf4.retrieve(500);
    assert(buf4.readableBytes() == 300);
    assert(buf4.writableBytes() == Buffer::kInitialSize - 800);
    assert(buf4.prependableBytes() == 8 + 500);

    buf4.append(std::string(300, '3'));
    assert(buf4.readableBytes() == 600);
    assert(buf4.writableBytes() == Buffer::kInitialSize - 600);
    assert(buf4.prependableBytes() == 8);
  }
  {
    Buffer buf5;
    buf5.append("11111", 5);
    assert(buf5.readableBytes() == 5);
    assert(buf5.prependableBytes() == 8);
    buf5.prependInt16(5);
    assert(buf5.prependableBytes() == 6);
    assert(buf5.readableBytes() == 5 + 2);
  }
  {
    Buffer buf6;
    const std::string name("mihooke");
    buf6.append(name);
    for (auto c : name) {
      assert(c == buf6.peekInt8());
      assert(c == buf6.readInt8());
    }
    auto array = {100, 200, 300, -4000, -5000, -6000};
    for (auto ele : array) {
      buf6.appendInt32(ele);
    }
    for (auto ele : array) {
      assert(buf6.readInt32() == ele);
    }
    assert(buf6.readableBytes() == 0);
  }
  {
    Buffer buf7;
    buf7.append("message1\r\nmessage2\r\n");
    auto crlf = buf7.findCRLF();
    assert(crlf != nullptr);
    std::string message1 = buf7.retrieveAsString(crlf);
    LOG_INFO << message1;
    buf7.retrieve(2);
    crlf = buf7.findCRLF();
    assert(crlf != nullptr);
    message1 = buf7.retrieveAsString(crlf);
    LOG_INFO << message1;
  }

  return 0;
}