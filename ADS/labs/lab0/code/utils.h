#ifndef UTILS_H
#define UTILS_H

#include <cstring>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <map>
#include <bitset>

namespace utils
{
    std::string getFileName(const std::string &fileName, const std::string &suffix){
        size_t final_pos = fileName.find_last_of("/\\");
        size_t suffix_pos = fileName.find_last_of('.');
        return final_pos == std::string::npos ? "./output/" + fileName.substr(0, suffix_pos) + suffix : "./output/" + fileName.substr(final_pos + 1, suffix_pos - final_pos - 1) + suffix;
    }
    std::string parseText(const std::string &input){
        std::ifstream file(input, std::ios::in | std::ios::binary); // 以二进制模式打开文件
        std::ostringstream oss;
        char buffer;
        while (file.get(buffer)) {
            oss << buffer; // 将读取的二进制数据写入字符串流
        }
        std::string content = oss.str();// 从字符串流中获取内容
        file.close(); // 关闭文件
        return content;
    }
    void output(const std::string &output, const std::string &data){
        std::ofstream file(output);
        if (file.is_open()) {
            file << data;
            file.close();
        }
    }
    std::string codingTable2String(const std::map<std::string, std::string> &codingTable){
        std::string result = "";
        for (auto it = codingTable.begin(); it != codingTable.end(); it++) {
            result += it->first + " " + it->second + "\n";
        }
        return result;
    }
    void loadCodingTable(const std::string &input, std::map<std::string, std::string> &codingTable){
        std::string text=parseText(input);
        int i=0,j=-1;
        while(j!=text.size()-1){
            i=text.find(' ',j+2);
            if(text[i+1]==' ') i++;
            std::string key=text.substr(j+1,i-j-1);
            j=text.find('\n',i+1);
            std::string value=text.substr(i+1,j-i-1);
            codingTable[key]=value;
        }
    }

    std::string compress(const std::map<std::string, std::string> &codingTable, const std::string &text)
    {   
        std::string compressed;
        compressed = "";
        for (int i = 0; i < text.length(); i++) {
            std::string key=text.substr(i,2);
            if(codingTable.find(text.substr(i,2))!=codingTable.end()){
                compressed += codingTable.at(key);
                i++;
            }
            else compressed += codingTable.at(key.substr(0,1));
        }
        std::string compressedBits = compressed;
        int numBits = compressedBits.length();
        int padding = (8 - numBits % 8) % 8;
        compressedBits.append(padding, '0');
        compressed="";
        for(int i=0;i<compressedBits.size();i+=8){
            std::string temp=compressedBits.substr(i,8);
            char c=static_cast<char>(std::stoi(temp,nullptr,2));
            compressed+=c;
        }
        std::bitset<64> numBitsBitSet(numBits);
        std::string numBitsStr = numBitsBitSet.to_string();
        std::string numBitsLE;
        for (int i = 0; i < 64; i += 8) {
            std::bitset<8> bits(numBitsStr.substr(i, 8));
            numBitsLE.insert(0, 1, (char)bits.to_ulong());
        }
        compressed = numBitsLE + compressed;
        return compressed;
    }
};

#endif