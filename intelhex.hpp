/** @brief Class to manipulate data from Intel hex file
* @author Tengiz Sharafiev <btolfa@gmail.com>
* @date 18.07.2015
*/

#pragma once

#include <string>
#include <istream>
#include <vector>

#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/numeric.hpp>

#include <boost/optional.hpp>

#include "IntelHexErrors.hpp"

namespace intelhex {

class IntelHex {
public:
    using record_t = std::vector<uint8_t>;

    enum class typerec_t : std::uint8_t {
        DataRecord = 0,
        EndofFileRecord,
        ExtendedSegmentAddressRecord,
        StartSegmentAddressRecor,
        ExtendedLinearAddressRecord,
        StartLinearAddressRecord
    };

    /// Построчно читаем содержимое потока в ihex
    friend std::istream &operator>>(std::istream &istream, IntelHex &ihex) {
        std::string line;
        while (std::getline(istream, line)) {
            ihex.checkCorrectnessOfLineOrThrow(line);
            ihex.parseLine(line);
        }
        return istream;
    }

    void checkCorrectnessOfLineOrThrow(std::string const &line) const {
        // Строка должна начинаться с :
        if (line.front() != ':') {
            throw HexRecordError("Decoding string should start with ':'");
        }

        // В строке должно быть нечётное количество символов : + по 2 символа на байт
        if (!(line.size() % 2)) {
            throw HexRecordError("Decoding string should have odd chars - : and even char for bytes");
        }

        if (line.size() < 11) {
            throw HexRecordError("Decoding string too short");
        }

        // Проверяем что в строке только допустимые символы - :, 0-9, a-f, A-F
        boost::for_each(line, [](const auto & ch){
            if ((ch == ':') || ((ch >= '0') && (ch <= '9')) || ((ch >= 'a') && (ch <= 'f')) || ((ch >= 'A') && (ch <= 'F'))) {
                throw HexRecordError("Decoding string have invalid char: " + ch);
            }
        });
    }

    std::uint8_t convertCharToByte(const char ch) const {
        if (ch >= '0' && ch <= '9') {
            return static_cast<std::uint8_t>(ch - '0');
        } else if (ch >= 'a' && ch <= 'f') {
            return static_cast<std::uint8_t>(ch - 'a' + 0xA);
        } else {
            return static_cast<std::uint8_t>(ch - 'A' + 0xA);
        }
    }

    template <typename Iterator>
    std::uint8_t convertStrToByte(Iterator const& itr) const {
        return (convertCharToByte(*itr) << 4) | convertCharToByte(*(itr+1));
    }

    std::vector <std::uint8_t> decodeRecord(const std::string &line) const {
        std::vector<std::uint8_t> result;
        for (auto itr = line.cbegin() + 1; itr != line.cend(); itr+=2) {
            result.emplace_back(convertStrToByte(itr));
        }
        return result;
    }

    std::uint8_t getRECLEN(record_t const& record) const {
        return record[0];
    }

    void checkTYPEREC(const std::uint8_t field) const {
        if (field > 0x05) {
            throw RecordTypeError(
                    std::string("Record type invalid: ") +
                    std::to_string(static_cast<std::uint16_t>(field)) +
                    " should be less 0x05");
        }
    }

    typerec_t getTYPEREC(record_t const& record) const {
        std::uint8_t field = record[3];
        checkTYPEREC(field);
        return static_cast<typerec_t>(field);
    }


    bool isCorrectChecksum(record_t const& record) const {
        return (boost::accumulate(record, 0) == 0);
    }

    void checkCorrectnessOfRecordOrThrow(record_t const &record) const {
        // Служебные данные занимают RECLEN(1) + OFFSET(2) + RECTYP(1) + CHKSUM(1) = 5 байт, данные дожны занимать RECLEN байт
        // т.е. длина вектора не должны быть равно 5 + RECLEN
        if (record.size() != (getRECLEN(record) + 5u)) {
            throw RecordLengthError(std::string("Record length invalid: ") + std::to_string(record.size()) +
                                            "but should be " + std::to_string(getRECLEN(record) + 5));
        }

        getTYPEREC(record);

        if (! isCorrectChecksum(record)) {
            throw RecordChecksumError("Record checksum invalid");
        }


    }

    void parseRecord(record_t const &record) {
        switch (getTYPEREC(record)) {
            case typerec_t::DataRecord:
                break;
            case typerec_t::EndofFileRecord:
                break;
            case typerec_t::ExtendedLinearAddressRecord:
                break;
            case typerec_t::ExtendedSegmentAddressRecord:
                break;
            case typerec_t::StartLinearAddressRecord:
                break;
            case typerec_t::StartSegmentAddressRecor:
                break;
        }
    }

    void parseLine(std::string const &line) {
        auto record = decodeRecord(line);
        checkCorrectnessOfRecordOrThrow(record);
        parseRecord(record);
    }

private:
    /// Stores segment base address of Intel HEX file.
    std::uint32_t segmentBaseAddress{0};

    struct StartSegmentAddress_t {
        /// content of the CS register
        std::uint16_t cs{0};
        /// content of the IP register
        std::uint16_t ip{0};
    };

    /// Stores the content of the CS/IP Registers, if used.
    boost::optional<StartSegmentAddress_t> startSegmentAddress;

    /// Stores the content of the EIP Register, if used.
    boost::optional<std::uint32_t> startLinearAddress;
};

}
