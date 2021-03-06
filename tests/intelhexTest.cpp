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

/// ������ ������� ��������� ��� ������������ ������
TEST_F(IntelHexTest, ShouldThrowIfInvalidDecodingString) {
    // ��� ������� ������
    EXPECT_THROW(intelhex::checkCorrectnessOfLineOrThrow("00"),intelhex::HexRecordError);
    // ������������ ����� ������
    EXPECT_THROW(intelhex::checkCorrectnessOfLineOrThrow(":0"),intelhex::HexRecordError);
    // ������������ ������
    EXPECT_THROW(intelhex::checkCorrectnessOfLineOrThrow(":?0"),intelhex::HexRecordError);
    // ������ ������ ��������
    EXPECT_THROW(intelhex::checkCorrectnessOfLineOrThrow(":0000"),intelhex::HexRecordError);
}

/** @brief ������ ������������ ������ �� ������ � ����� ����
*/
TEST_F(IntelHexTest, ShouldDecodeRecord) {
    std::string line = ":00000001FF";
    std::vector<std::uint8_t> expected {0x00,0x00, 0x00, 0x01, 0xFF};
    EXPECT_THAT(intelhex::decodeRecord(line), ::testing::ContainerEq(expected));
}

TEST_F(IntelHexTest, ShouldThrowIfInvalidRecord) {
    intelhex::IntelHex intelHex;

    EXPECT_THROW(intelHex.factoryRecord(":02000001FF")->checkOrThrow(), intelhex::RecordLengthError);
    EXPECT_THROW(intelHex.factoryRecord(":000000FFFF")->checkOrThrow(), intelhex::RecordTypeError);
    EXPECT_THROW(intelHex.factoryRecord(":0000000100")->checkOrThrow(), intelhex::RecordChecksumError);

    EXPECT_TRUE(intelhex::isCorrectChecksum(intelhex::decodeRecord(":1004E300CFF0FBE2FDF220FF20F2E120E2FBE6F396")));


    EXPECT_THROW(
        {
            intelhex::IntelHex ih;
            ih.parseLine(":1004E300CFF0FBE2FDF220FF20F2E120E2FBE6F396");
            ih.parseLine(":1004E300CFF0FBE2FDF220FF20F2E120E2FBE6F396");
                 }, intelhex::AddressOverlapError);

    EXPECT_THROW(
            {
                intelhex::IntelHex ih;
                ih.parseLine(":00000001FF");
                ih.parseLine(":00000001FF");
            }, intelhex::HexRecordError);
}

}