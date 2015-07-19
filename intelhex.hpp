/** @brief Class to manipulate data from Intel hex file
* @author Tengiz Sharafiev <btolfa@gmail.com>
* @date 18.07.2015
*/

#pragma once

#include <string>
#include <istream>
#include <vector>
#include <map>

#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/numeric.hpp>
#include <boost/range/adaptor/sliced.hpp>
#include <boost/range/adaptor/indexed.hpp>

#include <boost/optional.hpp>

#include "IntelHexErrors.hpp"

namespace intelhex {

using record_t = std::vector<std::uint8_t>;

enum class typerec_t : std::uint8_t {
    DataRecord = 0,
    EndOfFileRecord,
    ExtendedSegmentAddressRecord,
    StartSegmentAddressRecord,
    ExtendedLinearAddressRecord,
    StartLinearAddressRecord
};

void checkCorrectnessOfLineOrThrow(std::string const &line) {
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
    boost::for_each(line, [](const auto &ch) {
        if ((ch == ':') || ((ch >= '0') && (ch <= '9')) || ((ch >= 'a') && (ch <= 'f')) ||
            ((ch >= 'A') && (ch <= 'F'))) {
            throw HexRecordError("Decoding string have invalid char: " + ch);
        }
    });
}

std::uint8_t convertCharToByte(const char ch) noexcept {
    if (ch >= '0' && ch <= '9') {
        return static_cast<std::uint8_t>(ch - '0');
    } else if (ch >= 'a' && ch <= 'f') {
        return static_cast<std::uint8_t>(ch - 'a' + 0xA);
    } else {
        return static_cast<std::uint8_t>(ch - 'A' + 0xA);
    }
}

template<typename Iterator>
std::uint8_t convertStrToByte(Iterator itr) noexcept {
    return (convertCharToByte(*itr) << 4) | convertCharToByte(*(itr + 1));
}

/** @brief ConvertTo template function for convinient method to get data from record
 *
 */
template<typename T, typename Iterator>
inline typename std::enable_if<std::is_same<T, std::uint8_t>::value, T>::type convertTo(Iterator itr) noexcept {
    return *itr;
}

template<typename T, typename Iterator>
inline typename std::enable_if<std::is_same<T, std::uint16_t>::value, T>::type convertTo(Iterator itr) noexcept {
    return (static_cast<T>(*itr) << 8) | *(itr + 1);
}

template<typename T, typename Iterator>
inline typename std::enable_if<std::is_same<T, std::uint32_t>::value, T>::type convertTo(Iterator itr) noexcept {
    return (static_cast<T>(*itr) << 24) | (static_cast<T>(*(itr + 1)) << 16) | (static_cast<T>(*(itr + 2)) << 8) |  *(itr + 3);
}



std::vector<std::uint8_t> decodeRecord(const std::string &line) {
    std::vector<std::uint8_t> result;
    for (auto itr = line.cbegin() + 1; itr != line.cend(); itr += 2) {
        result.emplace_back(convertStrToByte(itr));
    }
    return result;
}

std::uint8_t getRECLEN(record_t const &record) noexcept {
    return record[0];
}

void checkTYPEREC(const std::uint8_t field) {
    if (field > 0x05) {
        throw RecordTypeError(
                std::string("Record type invalid: ") +
                std::to_string(static_cast<std::uint16_t>(field)) +
                " should be less 0x05");
    }
}

typerec_t getTYPEREC(record_t const &record) {
    std::uint8_t field = record[3];
    checkTYPEREC(field);
    return static_cast<typerec_t>(field);
}

std::uint16_t getOFFSET(record_t const &record) noexcept {
    return convertTo<std::uint16_t>(record.cbegin() + 1);
}

bool isCorrectChecksum(record_t const &record) {
    return (boost::accumulate(record, static_cast<std::uint8_t>(0)) == 0);
}

void checkRecordOrThrow(record_t const &record) {
// Служебные данные занимают RECLEN(1) + OFFSET(2) + RECTYP(1) + CHKSUM(1) = 5 байт, данные дожны занимать RECLEN байт
// т.е. длина вектора не должны быть равно 5 + RECLEN
    if (record.size() != (getRECLEN(record) + 5u)) {
        throw RecordLengthError(std::string("Record length invalid: ") + std::to_string(record.size()) +
                                "but should be " + std::to_string(getRECLEN(record) + 5));
    }

    getTYPEREC(record);

    if (!isCorrectChecksum(record)) {
        throw RecordChecksumError("Record checksum invalid");
    }
}

void checkExtendedSegmentAddressOrThrow(const record_t &record) {
    if (getRECLEN(record) != 2) {
        throw ExtendedSegmentAddressRecordError("Incorrect length of Extended Segment Address Record");
    }
}

void checkExtendedLinearAddressOrThrow(const record_t &record) {
    if (getRECLEN(record) != 2) {
        throw ExtendedLinearAddressRecordError("Incorrect length of Extended Linear Address Record");
    }
}

class IntelHex {
public:
    using content_t = std::map<std::uint32_t, std::uint8_t>;

    /// Построчно читаем содержимое потока в ihex
    friend std::istream &operator>>(std::istream &istream, IntelHex &ihex) {
        std::string line;
        while (std::getline(istream, line)) {
            checkCorrectnessOfLineOrThrow(line);
            ihex.parseLine(line);
        }
        return istream;
    }

    void saveByte(const std::uint32_t address, const std::uint8_t value) {
        if (content.emplace(std::make_pair(address, value)).second) {
            throw AddressOverlapError(
                    std::string("Trying to save to data with overlaped addressed: ") + std::to_string(address));
        }
    }

    void processData(const record_t &record) {
        /* Calculate new SBA by clearing the low four bytes and then adding the   */
        /* current loadOffset for this line of Intel HEX data                     */
        segmentBaseAddress &= ~(0xFFFFUL);
        segmentBaseAddress += getOFFSET(record);

        // save data from record to memory
        boost::for_each(record | boost::adaptors::sliced(4, 4 + getRECLEN(record)) |
                        boost::adaptors::indexed(segmentBaseAddress),
                        [this](const auto &element) {
                            this->saveByte(element.index(), element.value());
                        });

        segmentBaseAddress += getRECLEN(record);
    }

    void checkEndOfFileOrThrow(const record_t &record) const {
        if (record != record_t{00, 00, 00, 01, 0xFF}) {
            throw EOFRecordError("Invalid End Of File Record");
        }

        if (foundEOF) {
            throw HexRecordError("Additional End Of File record found");
        }
    }

    void processEndOfFile(const record_t &record) {
        checkEndOfFileOrThrow(record);
        foundEOF = true;
    }

    void processExtendedSegmentAddress(const record_t &record) {
        checkExtendedSegmentAddressOrThrow(record);

        /* ESA is bits 4-19 of the segment base address   */
        /* (SBA), so shift left 4 bits                    */
        segmentBaseAddress = convertTo<std::uint16_t>(record.cbegin() + 4) << 4;
    }

    void checkStartSegmentAddressOrThrow(const record_t &record) const {
        if (getRECLEN(record) != 4) {
            throw StartSegmentAddressRecordError("Incorrect length of Start Segment Address Record");
        }

        if (startSegmentAddress) {
            throw DuplicateStartAddressRecordError("Duplicate Start Segment Address Record");
        }

    }

    void processStartSegmentAddress(const record_t &record) {
        checkStartSegmentAddressOrThrow(record);
        startSegmentAddress.emplace(
                convertTo<std::uint16_t>(record.cbegin() + 4), // cs
                convertTo<std::uint16_t>(record.cbegin() + 6)  // ip
        );

    }

    void processExtendedLinearAddress(const record_t &record) {
        checkExtendedLinearAddressOrThrow(record);

        /* ELA is bits 16-31 of the segment base address  */
        /* (SBA), so shift left 16 bits                   */
        segmentBaseAddress = convertTo<std::uint16_t>(record.cbegin() + 4) << 16;
    }

    void processRecord(record_t const &record) {
        switch (getTYPEREC(record)) {
            case typerec_t::DataRecord:
                processData(record);
                break;
            case typerec_t::EndOfFileRecord:
                processEndOfFile(record);
                break;
            case typerec_t::ExtendedLinearAddressRecord:
                processExtendedLinearAddress(record);
                break;
            case typerec_t::ExtendedSegmentAddressRecord:
                processExtendedSegmentAddress(record);
                break;
            case typerec_t::StartLinearAddressRecord:
                break;
            case typerec_t::StartSegmentAddressRecord:
                processStartSegmentAddress(record);
                break;
        }
    }

    void parseLine(std::string const &line) {
        auto record = decodeRecord(line);
        checkRecordOrThrow(record);
        processRecord(record);
    }

private:
    /// Stores segment base address of Intel HEX file.
    std::uint32_t segmentBaseAddress{0};

    struct StartSegmentAddress_t {
        StartSegmentAddress_t(const std::uint16_t cs_, const std::uint16_t ip_) : cs{cs_}, ip{ip_} {}
        /// content of the CS register
        std::uint16_t cs{0};
        /// content of the IP register
        std::uint16_t ip{0};
    };

    /// Stores the content of the CS/IP Registers, if used.
    boost::optional<StartSegmentAddress_t> startSegmentAddress;

    /// Stores the content of the EIP Register, if used.
    boost::optional<std::uint32_t> startLinearAddress;

    content_t content;

    bool foundEOF{false};
};

}
