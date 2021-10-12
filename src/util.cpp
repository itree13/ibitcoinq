#include "util.h"
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <random>
#include <array>
#include <atomic>

std::string calcHmacSHA256(const std::string& msg, const std::string& decoded_key) {
    std::array<unsigned char, EVP_MAX_MD_SIZE> hash;
    unsigned int hashLen;

    HMAC(
        EVP_sha256(),
        decoded_key.data(),
        static_cast<int>(decoded_key.size()),
        reinterpret_cast<unsigned char const*>(msg.data()),
        static_cast<int>(msg.size()),
        hash.data(),
        &hashLen
    );

    return std::string{ reinterpret_cast<char const*>(hash.data()), hashLen };
}

std::string toDateTimeStr(uint64_t time_msec) {
    struct tm tstruct;
    char buf[80];
    time_t now = time_msec / 1000;
    tstruct = *localtime(&now);
    strftime(buf, _countof(buf), "%m-%d %H:%M:%S", &tstruct);
    return buf;
}

std::string toTimeStr(uint64_t time_msec) {
    struct tm tstruct;
    char buf[80];
    time_t now = time_msec / 1000;
    tstruct = *localtime(&now);
    strftime(buf, _countof(buf), "%H:%M:%S", &tstruct);
    return buf;
}

int generateRadomInt(int min_value, int max_value) {
    // genenrate random uuid of device
    static std::random_device r;
    static std::seed_seq seed{ r(), r(), r(), r(), r(), r(), r(), r() };
    static std::mt19937_64 eng(seed);

    std::uniform_int_distribution<int> dist{ min_value, max_value };
    return dist(eng);
}

std::string generateRandomString(size_t length) {
    static const std::string k_alpha = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZjJ";

    std::string s;
    s.reserve(length);
    for (size_t i = 0; i < length; ++i)
        s.push_back(k_alpha[generateRadomInt(0, (int)(k_alpha.size() - 1))]);
    return s;
}

void trimString(string& str) {
    if (str.empty())
        return;

    string spacechars = "\r\t\n ";
    str.erase(0, str.find_first_not_of(spacechars));
    spacechars.push_back('\0');
    str.erase(str.find_last_not_of(spacechars) + 1);
}

void splitString(const string& v, std::vector<string>& out, char delim, size_t max_seg) {
    string::size_type last_pos = 0;
    for (;;) {
        string s;
        if (out.size() >= max_seg - 1) {
            s = v.substr(last_pos);
            trimString(s);
            out.push_back(s);
            break;
        }

        string::size_type pos = v.find(delim, last_pos);
        if (pos == string::npos)
            s = v.substr(last_pos);
        else
            s = v.substr(last_pos, pos - last_pos);
        trimString(s);
        out.push_back(s);
        last_pos = pos + 1;
        if (pos == string::npos)
            break;
    }
}

std::string genCliOrdId() {
    static auto k_uni = generateRandomString(8);
    static std::atomic_int k_inc{ 0 };
    return k_uni + std::to_string(++k_inc);
}

int getCliOrdIdNumber(const std::string& cliordid) {
    return cliordid.size() > 8 ? strtol(cliordid.substr(8).c_str(), nullptr, 0) : 0;
}