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

    EXPECT_THROW(intelHex.checkCorrectnessOrThrow("00"),intelhex::HexRecordError);
    EXPECT_THROW(intelHex.checkCorrectnessOrThrow(":0"),intelhex::HexRecordError);
    EXPECT_THROW(intelHex.checkCorrectnessOrThrow(":?0"),intelhex::HexRecordError);
}



/** @brief Должен декодировать запись из строки в набор байт
*/
TEST_F(IntelHexTest, ShouldDecodeRecord) {
    intelhex::IntelHex intelHex;

    std::string line = ":00000001FF";
    std::vector<std::uint8_t> expected {0x00,0x00, 0x00, 0x01, 0xFF};
    EXPECT_THAT(intelHex.decodeRecord(line), ::testing::ContainerEq(expected));
}

}