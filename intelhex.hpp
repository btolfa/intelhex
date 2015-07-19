/** @brief Class to manipulate data from Intel hex file
* @author Tengiz Sharafiev <btolfa@gmail.com>
* @date 18.07.2015
*/

#pragma once

#include <string>
#include <istream>
#include <vector>

#include <boost/range/algorithm/for_each.hpp>

#include "IntelHexErrors.hpp"

namespace intelhex {

class IntelHex {
public:
    /// ��������� ������ ���������� ������ � ihex
    friend std::istream &operator>>(std::istream &istream, IntelHex &ihex) {
        std::string line;
        while (std::getline(istream, line)) {
            ihex.checkCorrectnessOrThrow(line);
            ihex.parseLine(line);
        }
        return istream;
    }

    void checkCorrectnessOrThrow(std::string const &line) const {
        // ������ ������ ���������� � :
        if (line.front() != ':') {
            throw HexRecordError("Decoding string should start with ':'");
        }

        // � ������ ������ ���� �������� ���������� �������� : + �� 2 ������� �� ����
        if (!(line.size() % 2)) {
            throw HexRecordError("Decoding string should have odd chars - : and even char for bytes");
        }

        // ��������� ��� � ������ ������ ���������� ������� - :, 0-9, a-f, A-F
        boost::for_each(line, [](const auto & ch){
            if ((ch == ':') || ((ch >= '0') && (ch <= '9')) || ((ch >= 'a') && (ch <= 'f')) || ((ch >= 'A') && (ch <= 'F'))) {
                throw HexRecordError("Decoding string have invalid char: " + ch);
            }
        });
    }

    std::uint8_t convertCharToByte(char ch) const {
        if (ch >= '0' && ch <= '9') {
            return static_cast<std::uint8_t>(ch - '0');
        } else if (ch >= 'a' && ch <= 'f') {
            return static_cast<std::uint8_t>(ch - 'a' + 0xA);
        } else {
            return static_cast<std::uint8_t>(ch - 'A' + 0xA);
        }
    }

    template <typename Iterator>
    std::uint8_t convertStrToByte(Iterator itr) const {
        return (convertCharToByte(*itr) << 4) | convertCharToByte(*(itr+1));
    }

    std::vector <std::uint8_t> decodeRecord(const std::string &line) const {
        std::vector<std::uint8_t> result;
        for (auto itr = line.cbegin() + 1; itr != line.cend(); itr+=2) {
            result.emplace_back(convertStrToByte(itr));
        }
        return result;
    }

    void parseLine(std::string const &line) {
        auto record = decodeRecord(line);
    }
};

}
