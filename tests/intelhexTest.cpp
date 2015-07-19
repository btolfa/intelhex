/**
* @author Tengiz Sharafiev <btolfa@gmail.com>
* @date 18.07.2015
*/

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "intelhex.hpp"

namespace {
class IntelHexTest : public ::testing::Test {

};

/// Должен бросить иключение для некорректной строки
TEST_F(IntelHexTest, ShouldThrowIfInvalidDecodingString) {
    intelhex::IntelHex intelHex;

    // Нет маркера записи
    EXPECT_THROW(intelHex.checkCorrectnessOfLineOrThrow("00"),intelhex::HexRecordError);
    // Неправильная длина строки
    EXPECT_THROW(intelHex.checkCorrectnessOfLineOrThrow(":0"),intelhex::HexRecordError);
    // Недопустимый символ
    EXPECT_THROW(intelHex.checkCorrectnessOfLineOrThrow(":?0"),intelhex::HexRecordError);
    // Запись лишком короткая
    EXPECT_THROW(intelHex.checkCorrectnessOfLineOrThrow(":0000"),intelhex::HexRecordError);
}

/** @brief Должен декодировать запись из строки в набор байт
*/
TEST_F(IntelHexTest, ShouldDecodeRecord) {
    intelhex::IntelHex intelHex;

    std::string line = ":00000001FF";
    std::vector<std::uint8_t> expected {0x00,0x00, 0x00, 0x01, 0xFF};
    EXPECT_THAT(intelHex.decodeRecord(line), ::testing::ContainerEq(expected));
}

TEST_F(IntelHexTest, ShouldThrowIfInvalidRecord) {
    intelhex::IntelHex intelHex;

    EXPECT_THROW(intelHex.checkCorrectnessOfRecordOrThrow(std::vector<std::uint8_t>{0x02, 0x00, 0x00, 0x01, 0xFF}), intelhex::RecordLengthError);
    EXPECT_THROW(intelHex.checkCorrectnessOfRecordOrThrow(std::vector<std::uint8_t>{0x00, 0x00, 0x00, 0xFF, 0xFF}), intelhex::RecordTypeError);
    EXPECT_THROW(intelHex.checkCorrectnessOfRecordOrThrow(std::vector<std::uint8_t>{0x00, 0x00, 0x00, 0x01, 0x00}), intelhex::RecordChecksumError);

    EXPECT_TRUE(intelHex.isCorrectChecksum(intelHex.decodeRecord(":1004E300CFF0FBE2FDF220FF20F2E120E2FBE6F396")));

    EXPECT_THROW({
        intelHex.parseLine(":1004E300CFF0FBE2FDF220FF20F2E120E2FBE6F396");
        intelHex.parseLine(":1004E300CFF0FBE2FDF220FF20F2E120E2FBE6F396");
                 }, intelhex::AddressOverlapError);
}

}