#include "gitbin/gitbin.h"
#include "io_utils/io_utils.h"
#include "log.h"

using namespace io_utils;

namespace gitbins {

void write_(FileWriter& out, uint8_t* buf, int n) {
  if (out.write(buf, 0, n) != n) {
    logerr << "write error" << logendl;
    exit(1);
  }
}

uint8_t g_encode_mapping[256];
uint8_t g_decode_mapping[256];
uint8_t g_encode_onebyte_mapping[256];
uint8_t g_decode_onebyte_mapping[256];

bool g_mapping_inited = []() {
  memset(g_encode_mapping, 255, sizeof(g_encode_mapping));
  memset(g_decode_mapping, 255, sizeof(g_decode_mapping));
  int j = 0;
  for (int i = 0; i < 10; i++) {
    g_encode_mapping[j] = '0' + i;
    g_decode_mapping['0' + i] = j++;
  }
  for (int i = 0; i < 26; i++) {
    g_encode_mapping[j] = 'a' + i;
    g_decode_mapping['a' + i] = j++;
  }
  for (int i = 0; i < 26; i++) {
    g_encode_mapping[j] = 'A' + i;
    g_decode_mapping['A' + i] = j++;
  }
  g_encode_mapping[j] = '+';
  g_decode_mapping[int('+')] = j++;
  g_encode_mapping[j] = '-';
  g_decode_mapping[int('-')] = j++;

  memset(g_encode_onebyte_mapping, 255, sizeof(g_encode_onebyte_mapping));
  memset(g_decode_onebyte_mapping, 255, sizeof(g_decode_onebyte_mapping));
  j = 0;
  g_encode_onebyte_mapping[j] = '!';
  g_decode_onebyte_mapping[int('!')] = j++;
  g_encode_onebyte_mapping[j] = '@';
  g_decode_onebyte_mapping[int('@')] = j++;
  g_encode_onebyte_mapping[j] = '#';
  g_decode_onebyte_mapping[int('#')] = j++;
  g_encode_onebyte_mapping[j] = '$';
  g_decode_onebyte_mapping[int('$')] = j++;
  return true;
}();

int encodeOneByte(uint8_t v, uint8_t* out) {
  *out++ = g_encode_onebyte_mapping[v>>6];
  *out = g_encode_mapping[v & 0x3F];
  return 2;
}

void encode(const std::string& in_file, const std::string& out_file) {
  FileReader in(in_file);
  if (!in.open()) exit(1);
  FileWriter out(out_file);
  if (!out.open()) exit(1);

  uint8_t rbuf[4 * 1024];
  uint8_t wbuf[2 * sizeof(rbuf)];
  int rpos = 0, ri = 0, wi = 0;
  while (true) {
    int rn = in.read(rbuf, rpos, sizeof(rbuf) - rpos);
    if (rn < 0) {
      logerr << "read error" << logendl;
      exit(1);
    } else if (rn == 0) {
      break;
    }
    rn += rpos;
    for (ri = 0; ri < rn; ri += 3) {
      wbuf[wi++] = g_encode_mapping[rbuf[ri] & 0x3F];
      wbuf[wi++] = g_encode_mapping[rbuf[ri+1] & 0x3F];
      wbuf[wi++] = g_encode_mapping[rbuf[ri+2] & 0x3F];
      wbuf[wi++] = g_encode_mapping[(rbuf[ri]>>6) | ((rbuf[ri+1]>>4)&0x0C) | ((rbuf[ri+2]>>2)&0x30)];
      if (wi >= 512) {
        write_(out, wbuf, wi);
        wi = 0;
      }
    }
    if (ri == rn) {
      rpos = 0;
    } else if (ri > rn) {
      ri -= 3;
      rpos = rn - ri;
      for (int i = 0; i < rpos; i++) {
        rbuf[i] = rbuf[ri+i];
      }
    }
  }
  if (rpos > 2) {
    logerr << "encode error: rpos(" << rpos << ") > 2" << logendl;
    exit(1);
  } else if (rpos == 2) {
    wi += encodeOneByte(rbuf[0], wbuf + wi);
    wi += encodeOneByte(rbuf[1], wbuf + wi);
  } else if (rpos == 1) {
    wi += encodeOneByte(rbuf[0], wbuf + wi);
  } else if (rpos < 0) {
    logerr << "encode error: rpos(" << rpos << ") < 0" << logendl;
    exit(1);
  }
  if (wi > 0) {
    write_(out, wbuf, wi);
    wi = 0;
  }

  logwarn << "encode: " << in_file << " ==> " << out_file << logendl;
}

void decode(const std::string& in_file, const std::string& out_file) {
  FileReader in(in_file);
  if (!in.open()) return;
  FileWriter out(out_file);
  if (!out.open()) return;

  logwarn << "decode: " << in_file << " ==> " << out_file << logendl;
}

void convert(const std::string& file) {
  if (startsWith(file, GitbinsFilePrefix)) {
    decode(file, file.substr(GitbinsFilePrefix.length()));
  } else {
    encode(file, GitbinsFilePrefix + file);
  }
}

}
