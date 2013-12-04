//=============================================================================
/**
 *  @file    CharsetUtils.h
 *
 *  ver 1.0.0 2004/06/16 Naven Exp, for Mime Message Parse Engine.
 *
 *  @author Naven 2004/06/16 created.
 */
//=============================================================================

#ifndef _CHARSETUTILS_H
#define _CHARSETUTILS_H

#if defined(_WIN32) || defined(__WIN32__)

#if !defined (NAVEN_PRAGMA_ONCE)
# pragma once
#endif /* NAVEN_PRAGMA_ONCE */

#endif /* _WIN32 */


#include "String.h"


#define _FASTMIME_USE_NAMESPACES  using namespace fastmime;
#define _FASTMIME fastmime
#define _FASTMIME_BEGIN_NAMESPACE namespace fastmime {
#define _FASTMIME_END_NAMESPACE }


_FASTMIME_BEGIN_NAMESPACE


#define DEFAULT_CONVERTER_LINE_LENGTH 0

#define WORD_SPECIALS   "=_?\"#$%&'(),.:;<>@[\\]^`{|}~"
#define TEXT_SPECIALS   "=_?"


class ICodeConverter
{
public:
    virtual void setLineLength(const int lineLength) = 0;
    virtual int encode(const FastString &input, FastString &output) = 0;
    virtual int decode(const FastString &input, FastString &output) = 0;
    virtual int encodedLength(const FastString &s, int encodingWord) = 0;
    virtual ~ICodeConverter(){};
};


/**
 * This class implements a BASE64 Encoder. It is implemented as
 * a FilterOutputStream, so one can just wrap this class around
 * any output stream and write bytes into this filter. The Encoding
 * is done as the bytes are written out.
 *
 * This class implements a BASE64 Decoder. It is implemented as
 * a FilterInputStream, so one can just wrap this class around
 * any input stream and read bytes from this filter. The decoding
 * is done as the bytes are read out.
 *
 */
class Base64Converter : public ICodeConverter
{
protected:
    int m_nLineLength;  // default 77, new set 0
public:
    Base64Converter() : m_nLineLength(DEFAULT_CONVERTER_LINE_LENGTH) { }
    void setLineLength(const int lineLength) {
        if( lineLength > 0 )
            m_nLineLength = lineLength;
        else
            m_nLineLength = DEFAULT_CONVERTER_LINE_LENGTH;
    }
    int encode(const FastString &input, FastString &output);
    int decode(const FastString &input, FastString &output);
    int encode2(const FastString &input, FastString &output);
    int decode2(const FastString &input, FastString &output);
    int encodedLength(const FastString &s, int encodingWord = 0);
    virtual ~Base64Converter(){};
};


/**
 * This class implements a Quoted Printable Encoder. It is implemented as
 * a FilterOutputStream, so one can just wrap this class around
 * any output stream and write bytes into this filter. The Encoding
 * is done as the bytes are written out.
 *
 * This class implements a QP Decoder. It is implemented as
 * a FilterInputStream, so one can just wrap this class around
 * any input stream and read bytes from this filter. The decoding
 * is done as the bytes are read out.
 *
 */
class QPConverter : public ICodeConverter
{
public:
    void setLineLength(const int lineLength) { }
    int encode(const FastString &input, FastString &output);
    int decode(const FastString &input, FastString &output);
    int encodedLength(const FastString &s, int encodingWord = 0);
    virtual ~QPConverter(){};
private:
    int nConvertHexToDec(int nFirstByte, int nSecondByte);
};


class ConverterFactory
{
public:
    enum{   BASE64  = 0,        // Base64
            B,                  // Base64
            QP,                 // Quoted-Printable
            Q,                  // Quoted-Printable
            URL,                // URL

            MAX_ENCODING
        };
public:
    static int encodedLength(const int encoding,
                      const FastString &s, int encodingWord = 0);
    static int encode(const int encoding,
                      const FastString &input, FastString &output);
    static int encode(const int encoding,
                      const FastString &input, FastString &output,
                      const int lineLength);
    static int decode(const int encoding,
                      const FastString &input, FastString &output);
    static int encode(ICodeConverter &encoder,
                      const FastString &input, FastString &output);
    static int encode(ICodeConverter &encoder,
                      const FastString &input, FastString &output,
                      const int lineLength);
    static int decode(ICodeConverter &encoder,
                      const FastString &input, FastString &output);
};


//============Inline Functions Implements================

/**
 *  Encode a string buffer with a specify encoding type.
 *
 *  @param encoding encoding type.
 *  @param input   input string.
 *  @param output  encode result string.
 *  @return  result string count.
 */
inline int ConverterFactory::encode(const int encoding,
                  const FastString &input, FastString &output)
{
    return ConverterFactory::encode(encoding, input, output, 0);
}

/**
 *  Encode a string buffer with a specify encoding type.
 *
 *  @param encoder code converter.
 *  @param input   input string.
 *  @param output  encode result string.
 *  @return  result string count.
 */
inline int ConverterFactory::encode(ICodeConverter &encoder,
                  const FastString &input, FastString &output)
{
    return encoder.encode(input, output);
}

/**
 *  Encode a string buffer with a specify encoding type.
 *  with line break.
 *
 *  @param encoder code converter.
 *  @param input   input string.
 *  @param output  encode result string.
 *  @param lineLength  line length where append line break.
 *  @return  result string count.
 */
inline int ConverterFactory::encode(ICodeConverter &encoder,
                  const FastString &input, FastString &output,
                  const int lineLength)
{
    encoder.setLineLength(lineLength);
    return encoder.encode(input, output);
}

/**
 *  Decode a code string buffer with a specify encoding type.
 *
 *  @param encoder code converter.
 *  @param input   input string.
 *  @param output  decode result string.
 *  @return  result string count.
 */
inline int ConverterFactory::decode(ICodeConverter &encoder,
                  const FastString &input, FastString &output)
{
    return encoder.decode(input, output);
}



_FASTMIME_END_NAMESPACE
_FASTMIME_USE_NAMESPACES

#endif
