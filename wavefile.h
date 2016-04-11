//
//  wavefile.h
//  wave-parser
//
//  Created by gustavo on 4/8/16.
//  Copyright © 2016 SEMB. All rights reserved.
//

#ifndef wavefile_h
#define wavefile_h


#endif /* wavefile_h */

typedef unsigned char byte;

/** Struct representing the header of a WAV file */
typedef struct wave_header {
    byte groupID[4];                            /// "RIFF"
    unsigned fileLength;                        ///
    byte sRiffType[4];                          /// "WAVE"
} WaveHeader;

/** Struct for keeping the metadata of the file */
typedef struct format_chunk {
    byte groupID[4];                            /// "fmt " - yes, with the whitespace
    unsigned chunkSize;                         /// Size of this chunk (not counting groupID)
    unsigned short formatTag;                   /// 01 For LPCM
    unsigned short numChannels;                 /// Number of channels (1 mono, 2 stereo)
    unsigned sampleRate;                        /// Sample Rate
    unsigned avgBytesPerSec;                    /// Sample Rate * Number of Channels * (Resolution/8)
    unsigned short blockAlign;                  ///
    unsigned bitsPerSample;                     /// Resolution
} FormatChunk;

/** Struct for containing the raw audio samples and some metadata */
typedef struct data_chunk {
    byte groupID[4];                            /// 'data'
    unsigned int chunkSize;                     /// RAW audio size
//    void *sampleData;                           /// byte/char for 8-bit, short for 16-bit, float for 32-bit
} DataChunk;