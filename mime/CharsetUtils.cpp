//=============================================================================
/**
 *  @file    CharsetUtils.cpp
 *
 *  ver 1.0.0 2004/06/16 Naven Exp, for Mime Message Parse Engine.
 *
 *  @author Naven 2004/06/16 created.
 */
//=============================================================================

#include "CharsetUtils.h"


_FASTMIME_BEGIN_NAMESPACE


/**
 *  4bit binary to char 0-F.
 *
 *  @param c  4bit character.
 *  @return   0-F character.
 */
static char Hex2Chr( unsigned char n )
{
    n &= 0xF;
    if ( n < 10 )
        return ( char )( n + '0' );
    else
        return ( char )( n - 10 + 'A' );
}

/**
 *  char 0-F to 4bit binary.
 *
 *  @param c  0-F character.
 *  @return   4bit character.
 */
static unsigned char Chr2Hex( char c )
{
    if ( c >= 'a' && c <= 'z' )  //  it's toupper
        c = c - 'a' + 'A';
    if ( c >= '0' && c <= '9' )
        return ( int )( c - '0' );
    else if ( c >= 'A' && c <= 'F' )
        return ( int )( c - 'A' + 10 );
    else
        return (unsigned char)(-1);
}

/**
 *  Base64 Character to Asiic Character.
 *  Base64 code table
 *  0-63 : A-Z(25) a-z(51), 0-9(61), +(62), /(63)
 *
 *  @param c  input base64 character.
 *  @return   asiic character.
 */
static char  Base2Chr( unsigned char n )
{
    n &= 0x3F;
    if ( n < 26 )
        return ( char )( n + 'A' );
    else if ( n < 52 )
        return ( char )( n - 26 + 'a' );
    else if ( n < 62 )
        return ( char )( n - 52 + '0' );
    else if ( n == 62 )
        return '+';
    else
        return '/';
}

/**
 *  Asiic Character to Base64 Character.
 *
 *  @param c  input asiic character.
 *  @return   base64 character.
 */
static unsigned char Chr2Base( char c )
{
    if ( c >= 'A' && c <= 'Z' )
        return ( unsigned char )( c - 'A' );
    else if ( c >= 'a' && c <= 'z' )
        return ( unsigned char )( c - 'a' + 26 );
    else if ( c >= '0' && c <= '9' )
        return ( unsigned char )( c - '0' + 52 );
    else if ( c == '+' )
        return 62;
    else if ( c == '/' )
        return 63;
    else
        return 64;  // ÎÞÐ§×Ö·û
}


/**
 * Returns the length of the encoded version of this byte array.
 */
int ConverterFactory::encodedLength(const int encoding,
                  const FastString &s, int encodingWord)
{
    switch( encoding )
    {
    case BASE64:
    case B:
        {
            Base64Converter base64;
            return base64.encodedLength(s, encodingWord);
        }
    case QP:
    case Q:
        {
            QPConverter qp;
            return qp.encodedLength(s, encodingWord);
        }
    case URL:
        {
            return -1;
        }
    }
    return 0;
}

/**
 *  Encode a string buffer with a specify encoding type.
 *  with line break.
 *
 *  @param encoding encoding type.
 *  @param input   input string.
 *  @param output  encode result string.
 *  @param lineLength  line length where append line break.
 *  @return  result string count.
 */
int ConverterFactory::encode(const int encoding,
                  const FastString &input, FastString &output,
                  const int lineLength)
{
    switch( encoding )
    {
    case BASE64:
    case B:
        {
            Base64Converter base64;
            base64.setLineLength(lineLength);
            return base64.encode(input, output);
        }
    case QP:
    case Q:
        {
            QPConverter qp;
            qp.setLineLength(lineLength);
            return qp.encode(input, output);
        }
    case URL:
        {
            output = input;
            return output.length();
        }
    }
    return 0;
}

/**
 *  Decode a code string buffer with a specify encoding type.
 *
 *  @param encoding encoding type.
 *  @param input   input string.
 *  @param output  decode result string.
 *  @return  result string count.
 */
int ConverterFactory::decode(const int encoding,
                  const FastString &input, FastString &output)
{
    switch( encoding )
    {
    case BASE64:
    case B:
        {
            Base64Converter base64;
            return base64.decode(input, output);
        }
    case QP:
    case Q:
        {
            QPConverter qp;
            return qp.decode(input, output);
        }
    case URL:
        {
            output = input;
            return output.length();
        }
    }
    return 0;
}

/**
 * Returns the length of the encoded version of this byte array.
 */
int Base64Converter::encodedLength(const FastString &s, int encodingWord)
{
    return ((s.length() + 2)/3) * 4;
}

/**
 * This character array provides the character to value map
 * based on RFC1521.
 */
static char base64_pem_array[64] =
{
    'A','B','C','D','E','F','G','H', // 0
    'I','J','K','L','M','N','O','P', // 1
    'Q','R','S','T','U','V','W','X', // 2
    'Y','Z','a','b','c','d','e','f', // 3
    'g','h','i','j','k','l','m','n', // 4
    'o','p','q','r','s','t','u','v', // 5
    'w','x','y','z','0','1','2','3', // 6
    '4','5','6','7','8','9','+','/'  // 7
};

static char base64_pem_convert_array[256] = {-1};

struct base64_init
{
    base64_init()
    {
        for( int i = 0; i < 64; i ++ )
            base64_pem_convert_array[(int)base64_pem_array[i]] = i;
    }
};

base64_init m_bGlobalBase64Init;

/**
 *  Base64 encode a string buffer.
 *  strlen(input)/strlen(output) = 3/4
 *
 * Base64 encode a byte array. With line breaks are inserted.
 * This method is suitable for short/long strings, such as those
 * in the IMAP AUTHENTICATE protocol, but not to encode the
 * entire content of a MIME part.
 *
 *  @param input   input string.
 *  @param output  encode result string.
 *  @return  result string count.
 */
int Base64Converter::encode(const FastString &input, FastString &output)
{
    int count = 0, line_count = 0;
    char buffer[5] = {0};
    int inpos = 0, lines = 0;
    int size = input.length();
    const char *p = input.c_str();

    if( m_nLineLength > 0 )
        lines = size / m_nLineLength + 2;
    output.clear();
    output.resize(((size + lines * 2 + 2) / 3) * 4);

    while( size > 0 )
    {
        char a, b, c;
        switch( size )
        {
        case 1:
            {
                a = p[inpos++];
                b = 0;
                c = 0;
                buffer[0] = (char)base64_pem_array[(a >> 2) & 0x3F];
                buffer[1] = (char)base64_pem_array[((a << 4) & 0x30) + ((b >> 4) & 0xf)];
                buffer[2] = (char)'=';  // pad character
                buffer[3] = (char)'=';  // pad character
            }
            break;

        case 2:
            {
                a = p[inpos++];
                b = p[inpos++];
                c = 0;
                buffer[0] = (char)base64_pem_array[(a >> 2) & 0x3F];
                buffer[1] = (char)base64_pem_array[((a << 4) & 0x30) + ((b >> 4) & 0xf)];
                buffer[2] = (char)base64_pem_array[((b << 2) & 0x3c) + ((c >> 6) & 0x3)];
                buffer[3] = (char)'=';  // pad character
            }
            break;

        default:
            {
                a = p[inpos++];
                b = p[inpos++];
                c = p[inpos++];
                buffer[0] = (char)base64_pem_array[(a >> 2) & 0x3F];
                buffer[1] = (char)base64_pem_array[((a << 4) & 0x30) + ((b >> 4) & 0xf)];
                buffer[2] = (char)base64_pem_array[((b << 2) & 0x3c) + ((c >> 6) & 0x3)];
                buffer[3] = (char)base64_pem_array[c & 0x3F];
            }
            break;
        }

        // m_nLineLength default = 77
        if( m_nLineLength > 0 && line_count+4 >= m_nLineLength )
        {
            output.append("\r\n", 2);
            line_count = 0;
        }

        output.append(buffer, 4);
        line_count += 4;

        count += 4;
        size -= 3;
    }

    return count;
}

/**
 *  Base64 decode a Base64 code string buffer.
 *  strlen(input)/strlen(output) = 4/3
 *
 *  @param input   input string.
 *  @param output  decode result string.
 *  @return  result string count.
 */
int Base64Converter::decode(const FastString &input, FastString &output)
{
    size_t count = 0;
    const char *start = 0, *p = 0;

    output.clear();
    if( input.empty() ) return 0;

    // cache of decoded bytes
    // 3 bytes cache data, last cahe end-char '\0'
    char buffer[4] = {0};
    char decode_buffer[4] = {0};
    int bufsize = 0;
    int input_length = input.length();
    start = p = input.c_str();

    output.resize(input_length*3/4 + 8);

    while( *p )
    {
        /*
         * We need 4 valid base64 characters before we start decoding.
         * We skip anything that's not a valid base64 character (usually
         * just CRLF).
         */
        int got = 0;
        while( got < 4 && *p ) {
            if( p - start >= input_length )
                break;
            int i = (int) *p++;
            if( i == '\r' || i == '\n' || i == ' ' || i == '\t' )
                continue;
            if( i >= 0 && i < 256 && (i == '=' || base64_pem_convert_array[i] != -1) )
                decode_buffer[got++] = (char) i;
        }
        if( got != 4 )
            break;

        int a = base64_pem_convert_array[decode_buffer[0] & 0xff];
        int b = base64_pem_convert_array[decode_buffer[1] & 0xff];
        // The first decoded byte
        buffer[bufsize++] = (char)(((a << 2) & 0xfc) | ((b >> 4) & 3));

        if( decode_buffer[2] == '=' ) // End of this BASE64 encoding
            break;
        int c = base64_pem_convert_array[decode_buffer[2] & 0xff];
        // The second decoded byte
        buffer[bufsize++] = (char)(((b << 4) & 0xf0) | ((c >> 2) & 0xf));

        if( decode_buffer[3] == '=' ) // End of this BASE64 encoding
            break;
        int d = base64_pem_convert_array[decode_buffer[3] & 0xff];
        // The third decoded byte
        buffer[bufsize++] = (char)(((c << 6) & 0xc0) | (d & 0x3f));

        output.append(buffer, bufsize);
        count += bufsize;
        bufsize = 0;
    }

    if( bufsize > 0 )
    {
        buffer[bufsize] = 0;
        output.append(buffer, bufsize);
        count += bufsize;
    }

    return count;
}

/**
 *  Base64 encode a string buffer.
 *  strlen(input)/strlen(output) = 3/4
 *
 *  @param input   input string.
 *  @param output  encode result string.
 *  @return  result string count.
 */
int Base64Converter::encode2(const FastString &input, FastString &output)
{
    const static char Base64Code[] =
        {"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"};

    size_t i = 0, n = 0, count = 0;
    char caThree[3] = {0}, caFour[5] = {0};
    const char *p = NULL;

    output.clear();

    if( input.empty() )
        return 0;

    output.resize(input.length()*4/3 + 8);

    for( n = 0, p = input.c_str(); (*p != '\0') && n < input.length(); )
    {
        //memset(caThree, 0, sizeof(caThree));
        caThree[0] = caThree[1] = caThree[2] = '\0';
        caFour[0]  = caFour[1]  = caFour[2]  = caFour[3] = '\0';

        for( i = 0; ( (i < 3) && (*p != '\0') && n < input.length() ); n ++ )
            caThree[i++] = *p++;

        // m_nLineLength default = 77
        if( this->m_nLineLength > 0 && (count != 0) &&
          ( count % this->m_nLineLength == 0 ) )
            output.append("\r\n", 2);

        count += 3;

        caFour[0] = Base64Code[((caThree[0] >> 2) & 0x3f)];
        caFour[1] = Base64Code[(((caThree[0] & 0x03) << 4) | ((caThree[1] >> 4) & 0x0f))];

        if( i < 2 )
            caFour[2] = '=';
        else
            caFour[2] = Base64Code[(((caThree[1] & 0x0f) << 2) | ((caThree[2] >> 6) &0x03))];

        if( i < 3 )
            caFour[3] = '=';
        else
            caFour[3] = Base64Code[(caThree[2] & 0x3f)];

        output.append(caFour, 4);
    }

    return count;
}

/**
 *  Base64 decode a Base64 code string buffer.
 *  strlen(input)/strlen(output) = 4/3
 *
 *  @param input   input string.
 *  @param output  decode result string.
 *  @return  result string count.
 */
int Base64Converter::decode2(const FastString &input, FastString &output)
{
    size_t i = 0, n = 0, count = 0;
    char caFour[4] = {0}, caThree[4] = {0}, c = 0;
    const char *p = NULL;

    output.clear();
    if( input.empty() ) return 0;
    output.resize(input.length()*3/4 + 8);

    for( n = 0, p = input.c_str(); (*p != '\0') && n < input.length(); )
    {
        //memset(caFour, 0, sizeof(caFour));
        caFour[0]  = caFour[1]  = caFour[2]  = caFour[3] = 0;
        caThree[0] = caThree[1] = caThree[2] = 0;

        for( i = 0; ((i < 4) && (*p != '\0') && n < input.length()); )
        {
            c = *p ++;
            if( c >= 'A' && c <= 'Z')
                c -= 65;
            else if( c >= 'a' && c <= 'z' )
                c -= 71 ;
            else if( c >= '0' && c <= '9' )
                c += 4;
            else if( c == '+' )
                c = 62;
            else if( c == '/' )
                c = 63;
            else if( c == '=' )
                c = 0;
            else
                continue;

            caFour[i++] = c;
            n ++;
        }

        caThree[0] = (((caFour[0] & 0x3f)<<2) | ((caFour[1] & 0x3f)>>4));
        caThree[1] = (((caFour[1] & 0x3f)<<4) | ((caFour[2] & 0x3f)>>2));
        caThree[2] = (((caFour[2] & 0x3f)<<6) | (caFour[3] & 0x3f));

        output.append(caThree, 3);
        count += 3;
    }

    return count;
}

/**
 * Returns the length of the encoded version of this byte array.
 */
int QPConverter::encodedLength(const FastString &s, int encodingWord)
{
    int len = 0, c = 0; char chr = 0;
    FastString specials = encodingWord ? WORD_SPECIALS : TEXT_SPECIALS;
    for( int i = 0; i < (int) s.length(); i++ )
    {
        chr = s.charAt(i);
        c = chr & 0xff; // Mask off MSB
        if( c < 040 || c >= 0177 || specials.indexOf(chr) >= 0 )
            // needs encoding
            len += 3; // Q-encoding is 1 -> 3 conversion
        else
            len ++;
    }
    return len;
}

/**
 *  Quoted-Printable encode a string buffer.
 *
 *  @param input   input string.
 *  @param output  encode result string.
 *  @return  result string count.
 */
int QPConverter::encode(const FastString &input, FastString &output)
{
    const char *q = input.c_str();
    char three[4] = {0}, c = 0;
    size_t i = 0, count = 0;
    int gotSpace = 0, gotCR = 0;

    output.clear();
    if( input.empty() ) return 0;
    output.resize(input.length() * 3);

    while ( i++ < input.length() )
    {
        c = *q ++;

        // Turn off the MSB.
        c = c & 0xff;

        // previous character was <SPACE>
        if( gotSpace )
        {
            if( c == '\r' || c == '\n' )
            {
                // if CR/LF, we need to encode the <SPACE> char
                three[0] = '=';
                three[1] = Hex2Chr( ' ' >> 4 );
                three[2] = Hex2Chr( ' ' );
                output.append(three, 3);
                count += 3;
            }
            else
            {
                // no encoding required, just output the char
                output.append(' ');
                count ++;
            }
            gotSpace = 0;
        }

        if( c == '\r' )
        {
            gotCR = 1;
            output.append("\r\n", 2);
            count += 2;
        }
        else
        {
            if( c == '\n' )
            {
                if( gotCR )
                    // This is a CRLF sequence, we already output the
                    // corresponding CRLF when we got the CR, so ignore this
                    ;
                else
                {
                    output.append("\r\n", 2);
                    count += 2;
                }
            }
            else if ( c == ' ' )
            {
                gotSpace = 1;
            }
            else if ( c < 040 || c >= 0177 || c == '=' )
            {
                // Encoding required.
                three[0] = '=';
                three[1] = Hex2Chr( c >> 4 );
                three[2] = Hex2Chr( c );
                output.append(three, 3);
                count += 3;
            }
            else
            {
                // No encoding required
                output.append(c);
                count ++;
            }
            // whatever it was, it wasn't a CR
            gotCR = 0;
        }
    }

    return count;
}

/**
 *  Quoted-Printable decode a encoded string buffer.
 *
 *  @param input   input string.
 *  @param output  decode result string.
 *  @return  result string count.
 */
int QPConverter::decode(const FastString &input, FastString &output)
{
    size_t   i = 0, n = 0, len = input.length();
    unsigned char  ch = 0, cl = 0;
    const char  *start = NULL, *aSrc = NULL;

    output.clear();
    if( input.empty() ) return 0;
    output.resize(input.length());

    start = aSrc = input.c_str();

    // aSrc is an ASCIIZ string
    while ( *aSrc && (aSrc - start) < (int) input.length() )
    {
        char c = 0;
        if ( ( *aSrc == '=' ) && ( input.length() - 2 > 0 ) )
        {
            ch = Chr2Hex( aSrc[1] );
            cl = Chr2Hex( aSrc[2] );
            if( strncmp( aSrc, "=\r\n", 3) == 0)
            {
                aSrc += 3;
                continue;
            }else if( !(aSrc[1]) )
            {
                aSrc++;
                continue;
            }else if ( ( ch == ( unsigned char )-1 ) || ( cl == ( unsigned char )-1 ) )
            {
                      c = *aSrc++;
            }
                    else
                    {
                        c = ( ch << 4 ) | cl;
                        aSrc += 3;
                    }
        }
        else
            c = *aSrc++;
        output.append(c);
        n ++;

    }
    return n;
}


_FASTMIME_END_NAMESPACE

