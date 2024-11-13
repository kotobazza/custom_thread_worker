#include "HashFunctions.h"
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>
#include <openssl/md5.h>




std::string computeHashSHA256(const std::string& input) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(input.c_str()), input.size(), hash);

    std::stringstream ss;
    for(int i =0; i< SHA256_DIGEST_LENGTH; i++){
        ss<<std::setfill('0')<<std::setw(2)<<std::hex<<(int)hash[i];
    }
    
    return ss.str();
}

std::string computeHashMD5(const std::string& input) {
    unsigned char hash[MD5_DIGEST_LENGTH];
    MD5(reinterpret_cast<const unsigned char*>(input.c_str()), input.size(), hash);


    std::stringstream ss;
    for(int i =0; i< MD5_DIGEST_LENGTH; i++){
        ss<<std::setfill('0')<<std::setw(2)<<std::hex<<(int)hash[i];
    }
    
    return ss.str();
}

