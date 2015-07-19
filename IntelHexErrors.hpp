/** @brief Exception for intelhex class
 * base on http://pythonhosted.org/IntelHex/appendix-a.html
 *
 *
    IntelHexError - base error
        HexReaderError - general hex reader error
            AddressOverlapError - data for the same address overlap
            HexRecordError - hex record decoder base error
                RecordLengthError - record has invalid length
                RecordTypeError - record has invalid type (RECTYP)
                RecordChecksumError - record checksum mismatch
                EOFRecordError - invalid EOF record (type 01)
                ExtendedAddressRecordError - extended address record base error
                    ExtendedSegmentAddressRecordError - invalid extended segment address record (type 02)
                    ExtendedLinearAddressRecordError - invalid extended linear address record (type 04)
                StartAddressRecordError - start address record base error
                    StartSegmentAddressRecordError - invalid start segment address record (type 03)
                    StartLinearAddressRecordError - invalid start linear address record (type 05)
                    DuplicateStartAddressRecordError - start address record appears twice
                    InvalidStartAddressValueError - invalid value of start addr record
        BadAccess16bit - not enough data to read 16 bit value
        NotEnoughDataError - not enough data to read N contiguous bytes
        EmptyIntelHexError - requested operation cannot be performed with empty object

 *
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

/// general hex reader error
class HexReaderError : public IntelHexError {
public:
    virtual ~HexReaderError() = default;
    using IntelHexError::IntelHexError;
};

/// data for the same address overlap
class AddressOverlapError : public HexReaderError {
public:
    using HexReaderError::HexReaderError;
};

/// hex record decoder base error
class HexRecordError : public HexReaderError {
public:
    virtual ~HexRecordError() = default;
    using HexReaderError::HexReaderError;
};

/// record has invalid length
class RecordLengthError : public HexRecordError {
public:
        using HexRecordError::HexRecordError;
};

/// record has invalid type (RECTYP)
class RecordTypeError : public HexRecordError {
public:
        using HexRecordError::HexRecordError;
};

/// record checksum mismatch
class RecordChecksumError : public HexRecordError {
public:
        using HexRecordError::HexRecordError;
};

/// invalid EOF record (type 01)
class EOFRecordError : public HexRecordError {
public:
        using HexRecordError::HexRecordError;
};

/// extended address record base error
class ExtendedAddressRecordError : public HexRecordError {
public:
        virtual ~ExtendedAddressRecordError() = default;
        using HexRecordError::HexRecordError;
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
class StartAddressRecordError : public HexRecordError {
public:
        virtual ~StartAddressRecordError() = default;
        using HexRecordError::HexRecordError;
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

