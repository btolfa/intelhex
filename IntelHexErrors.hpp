/** @brief Exception for intelhex class
 * base on http://pythonhosted.org/IntelHex/appendix-a.html
* @author Tengiz Sharafiev <btolfa@gmail.com>
* @date 19.07.2015
*/

#pragma once

#include <stdexcept>

namespace intelhex {

/// base error
class IntelHexError : public std::runtime_error {
public:
    virtual ~IntelHexError() = default;
    using std::runtime_error::runtime_error;
};

/// general hex reader error
class HexReaderError : public IntelHexError {
public:
    virtual ~HexReaderError() = default;
    using IntelHexError::IntelHexError;
};

/// not enough data to read 16 bit value
class BadAccess16bit : public IntelHexError {
public:
        using IntelHexError::IntelHexError;
};

/// not enough data to read N contiguous bytes
class NotEnoughDataError : public IntelHexError {
public:
        using IntelHexError::IntelHexError;
};

/// requested operation cannot be performed with empty object
class EmptyIntelHexError : public IntelHexError {
public:
        using IntelHexError::IntelHexError;
};

/// record has invalid length
class RecordLengthError : public HexReaderError {
public:
        using HexReaderError::HexReaderError;
};

/// record has invalid type (RECTYP)
class RecordTypeError : public HexReaderError {
public:
        using HexReaderError::HexReaderError;
};

/// record checksum mismatch
class RecordChecksumError : public HexReaderError {
public:
        using HexReaderError::HexReaderError;
};

/// invalid EOF record (type 01)
class EOFRecordError : public HexReaderError {
public:
        using HexReaderError::HexReaderError;
};

/// extended address record base error
class ExtendedAddressRecordError : public HexReaderError {
public:
        virtual ~ExtendedAddressRecordError() = default;
        using HexReaderError::HexReaderError;
};

/// invalid extended segment address record (type 02)
class ExtendedSegmentAddressRecordError : public ExtendedAddressRecordError {
public:
        using ExtendedAddressRecordError::ExtendedAddressRecordError;
};

/// invalid extended linear address record (type 04)
class ExtendedLinearAddressRecordError : public ExtendedAddressRecordError {
public:
        using ExtendedAddressRecordError::ExtendedAddressRecordError;
};

/// start address record base error
class StartAddressRecordError : public HexReaderError {
public:
        virtual ~StartAddressRecordError() = default;
        using HexReaderError::HexReaderError;
};

/// invalid start segment address record (type 03)
class StartSegmentAddressRecordError : public StartAddressRecordError {
public:
        using StartAddressRecordError::StartAddressRecordError;
};

/// invalid start linear address record (type 05)
class StartLinearAddressRecordError : public StartAddressRecordError {
public:
        using StartAddressRecordError::StartAddressRecordError;
};

/// start address record appears twice
class DuplicateStartAddressRecordError : public StartAddressRecordError {
public:
        using StartAddressRecordError::StartAddressRecordError;
};

/// invalid value of start addr record
class InvalidStartAddressValueError : public StartAddressRecordError {
public:
        using StartAddressRecordError::StartAddressRecordError;
};

}

