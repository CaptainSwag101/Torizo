#include "compression.h"

QByteArray DecompressData(QByteArray source, int maxDataSize)
{
    QByteArray output;
    
    QDataStream stream(source);
    stream.setByteOrder(QDataStream::LittleEndian);

    long bytesWritten = 0;
    while (bytesWritten < maxDataSize && !stream.atEnd())
    {
        long iterationPos = stream.device()->pos();

        uchar raw;
        stream >> raw;

        if (raw == 0xFF)    // Termination code
            break;

        uchar cmd = (raw & 0b11100000) >> 5;
        ushort val = raw & 0b00011111;

        if (cmd == 0b111)   // Extended command
        {
            cmd = val >> 2;
            uchar raw2;
            stream >> raw2;
            ushort val2 = ((val & 0b00000011) << 8) | raw2;
            val = val2;
        }

        switch (cmd)
        {
        case 0: // Direct Copy
        {
            // Copy (val + 1) bytes to the output
            long curPos = stream.device()->pos();
            for (ushort b = 0; b < (val + 1); ++b)
            {
                uchar byte;
                stream >> byte;
                output.append(byte);
                ++bytesWritten;

                if (bytesWritten >= maxDataSize)
                    break;
            }
            break;
        }
        case 1: // Byte Fill
        {
            // Writes the next byte (val + 1) bytes deep into the output
            long curPos = stream.device()->pos();
            uchar fill;
            stream >> fill;
            for (ushort b = 0; b < (val + 1); ++b)
            {
                output.append(fill);
                ++bytesWritten;

                if (bytesWritten >= maxDataSize)
                    break;
            }
            break;
        }
        case 2: // Word Fill
        {
            // Writes the next word (val + 1) bytes deep into the output
            long curPos = stream.device()->pos();
            uchar fill1;
            uchar fill2;
            stream >> fill1;
            stream >> fill2;
            for (ushort b = 0; b < (val + 1); ++b)
            {
                output.append(fill1);
                ++bytesWritten;
                ++b;

                if (bytesWritten >= maxDataSize || b >= (val + 1))
                    break;

                output.append(fill2);
                ++bytesWritten;

                if (bytesWritten >= maxDataSize)
                    break;
            }
            break;
        }
        case 3: // Ascending Fill
        {
            // Writes the next byte to the output, then increments that byte by 1
            // and writes it again, etc. Writes (val + 1) times.
            long curPos = stream.device()->pos();
            uchar fill;
            stream >> fill;
            for (ushort b = 0; b < (val + 1); ++b)
            {
                output.append(fill++);
                ++bytesWritten;

                if (bytesWritten >= maxDataSize)
                    break;
            }
            break;
        }
        case 4: // Indirect Copy
        {
            // Copies (val + 1) bytes from the output address in the next two bytes
            long curPos = stream.device()->pos();
            ushort indirectAddr;
            stream >> indirectAddr;
            for (ushort b = 0; b < (val + 1); ++b)
            {
                output.append(output.at(indirectAddr + b));
                ++bytesWritten;

                if (bytesWritten >= maxDataSize)
                    break;
            }
            break;
        }
        case 5: // Negated Indirect Copy
        {
            // Similar to Indirect Copy, but the copied data is eXclusive ORed with 0xFF, negating it
            long curPos = stream.device()->pos();
            ushort libraryAddr;
            stream >> libraryAddr;
            for (ushort b = 0; b < (val + 1); ++b)
            {
                output.append(output.at(libraryAddr + b) ^ 0xFF);
                ++bytesWritten;

                if (bytesWritten >= maxDataSize)
                    break;
            }
            break;
        }
        case 6: // Sliding Dictionary
        {
            // Copies decompressed data beginning at 'reverseOffset' bytes before the current position,
            // and copies (val + 1) bytes.
            long curPos = stream.device()->pos();
            uchar reverseOffset;
            stream >> reverseOffset;
            for (ushort b = 0; b < (val + 1); ++b)
            {
                int outputOffset = output.size() - reverseOffset;
                output.append(output.at(outputOffset));
                ++bytesWritten;

                if (bytesWritten >= maxDataSize)
                    break;
            }
            break;
        }
        case 7: // Negated Sliding Dictionary
        {
            // Same as Sliding Dictionary, but copied data is eXclusive ORed with 0xFF, negating it
            long curPos = stream.device()->pos();
            uchar reverseOffset;
            stream >> reverseOffset;
            for (ushort b = 0; b < (val + 1); ++b)
            {
                int outputOffset = output.size() - reverseOffset;
                output.append(output.at(outputOffset) ^ 0xFF);
                ++bytesWritten;

                if (bytesWritten >= maxDataSize)
                    break;
            }
            break;
        }
        default:
        {
            // This code should be impossible to hit,
            // due to the math used to compute the compression command
            throw "Invalid compression command!";
        }
        }
    }

    return output;
}

QByteArray CompressData(QByteArray source)
{
    QByteArray output;
    
    throw "Not Implemented";

    // This is probably gonna be super messy for the time being,
    // I'm not super great at writing compression heuristics.

    // We've got a lot of different commands we can use to compress data in Super Metroid,
    // from deduplicating immediately repeating bytes/words to finding
    // increasing sequences of bytes, or copying previous sequences
    // and performing various transformations on them.
    // And we need to find a good priority for each of them in the case where multiple
    // commands would work equally or almost equally well.

    // This will be a two-step process.
    // Part 1: Try compressing the sequence with each method, and store the resulting sequence that can be compressed
    // Part 2: Compare how long each compressable sequence is, and choose the method that provided the best compression.
    // If no method is able to compress the data, add a byte to the Direct Copy sequence and start a new initial sequence.
    // Once we find a valid compression for the current byte, add the Direct Copy command + array to the compressed output,
    // and then append the newly found compression method next.

    return output;
}
