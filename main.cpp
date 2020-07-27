#include <iostream>

#include "h264decoder.h"

#define MAX_INPUT_FILE_SIZE  (30 * 1024 * 1024)
#define FILE_READ_BUFFER     (600 * 1024)
#define ANNEX_B_START_CODE   (0x01000000)

#define NALU_TYPE_SPS        (0x07)
//#define NALU_TYPE_PPS      (0x08)
#define NALU_TYPE_SEI        (0x06)
#define NALU_TYPE_AUD        (0x09)
#define NALU_TYPE_IDR        (0x05)
#define NALU_TYPE_NON_IDR    (0x01)

#define NALU_TYPE_MASK       (0x1f)
//#define NALU_NRI_MASK      (0x60)

uint8_t* g_h264File;
uint32_t g_h264FileSize; 
uint32_t g_fileOffset;

//Read the entire file into memory
static int readFile(const char* file) {
    struct stat fileStat;
    if (stat(file, &fileStat) == -1) {
        printf ("Error: unable to open %s!!\n", file);
        exit(1);
    }
    if (fileStat.st_size <= 0) {
        printf ("Error: File %lu size is invalid !!\n", fileStat.st_size);
        exit(1);
    }

    g_h264FileSize = (unsigned long)fileStat.st_size;
    if (g_h264FileSize > MAX_INPUT_FILE_SIZE) {
        printf("Warning: File %s is too big, truncating ...\n", file);
        g_h264FileSize = MAX_INPUT_FILE_SIZE;
    }
    //printf ("%s size: %d\n", file, g_h264FileSize);

    static uint8_t readBuff[FILE_READ_BUFFER];  
    g_h264File = (uint8_t*)malloc(g_h264FileSize);

    FILE* fp = fopen(file, "rb");
    if (fp == NULL) {
        printf ("Error: unable to open %s!!\n", file);
        exit(1);
    }    
    
    unsigned long bytesRead = 0;
    uint32_t fileBufferSize = 0;
    while((bytesRead = fread(readBuff, 1, FILE_READ_BUFFER, fp)) > 0) {
        if (fileBufferSize + bytesRead <= g_h264FileSize) {
            memcpy (g_h264File + fileBufferSize, readBuff, bytesRead);
            fileBufferSize += bytesRead;
        } else {
            break;
        }
    }
    return 0;
}


//Read H.264 frame from the file buffer
static int readFrame(uint8_t** frame) {
    uint32_t i = g_fileOffset;
    int start = -1;
    int end = -1;
    uint8_t found = 0;
    for (; i < g_h264FileSize - 4; i++) {
        if (*((uint32_t*)(g_h264File + i)) == ANNEX_B_START_CODE) {
            uint8_t type = *(g_h264File + i + 4) & NALU_TYPE_MASK;
            // uint8_t nri = *(g_h264File + i + 4) & NALU_NRI_MASK;
            if (type == NALU_TYPE_SPS || type == NALU_TYPE_AUD || type == NALU_TYPE_SEI) {
                if (found) {
                    end = (int)i;
                    break;
                } else {
                    if (start == -1)
                        start = (int)i;
                }
            } else if (type == NALU_TYPE_NON_IDR) {
                if (found) {
                    end = (int)i;
                    break;
                } else {
                    start = (int)i;
                    found = 1;
                }
            } else if (type == NALU_TYPE_IDR) {
                if (start != -1)
                    found = 1;
            } else {
                //printf ("Type: %0x\n", type);
            }
        }
    }
    if (start != -1 && found == 1 && i >= (g_h264FileSize - 4))
        end = (int)g_h264FileSize;
    int size = end - start;
    if (size > 0) {
        //printf ("Frame size: %d\n", size);
        //memcpy (frame, g_h264File + start, size);
        *frame = g_h264File + start;
        g_fileOffset = (uint32_t)end;
    }
    //printf ("readFrame(%d, %d) -\n", g_fileOffset, size);
    if (g_fileOffset >= g_h264FileSize)
        g_fileOffset = 0;

    return size;
} 


int main(int argc, char** argv) {	
	readFile(argv[1]);
	
	
	int32_t width = atoi(argv[2]);
	int32_t height = atoi(argv[3]);
	
	RawH264Frame h264Frame;
	h264Frame.data = new unsigned char[H264_INBUF_SIZE];
	
	RGBAFrame rgbaFrame;
	rgbaFrame.data = new unsigned char[width * height * 4];
	H264Decoder decoder(width, height);
	
	int count = 0;
	
	uint32_t totalTime = 0;
	int inputFrameCount = 0;
	int outputFrameCount = 0;

	std::chrono::steady_clock::time_point startTime;
	std::chrono::steady_clock::time_point endTime;

	std::chrono::steady_clock::time_point totalStartTime;
	std::chrono::steady_clock::time_point totalEndTime;

	totalStartTime = std::chrono::steady_clock::now();
	int totalFrameCount = 0;
	while (1) {	
	
		startTime = std::chrono::steady_clock::now();
	
        UINT8* frame = NULL;	
        
		UINT32 rgbaSize = 0;
		h264Frame.dataSize = readFrame(&h264Frame.data);
		decoder.decodeFrame(&h264Frame, &rgbaFrame);
		inputFrameCount++;
		outputFrameCount++;
		totalFrameCount++;
		
		endTime = std::chrono::steady_clock::now();
		totalTime += std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
		if (outputFrameCount >= 1000) {
			totalEndTime = std::chrono::steady_clock::now();
			UINT32 duration = std::chrono::duration_cast<std::chrono::milliseconds>(totalEndTime - totalStartTime).count();
			std::cout << "Avg. Decoding Time = " << totalTime / outputFrameCount << ", Avg. FPS = " << outputFrameCount / (duration / 1000) << std::endl;
			totalTime = 0;
			inputFrameCount = outputFrameCount = 0;
			
			//char name[64];
			//sprintf(name, "out_%d.rgba", totalFrameCount);
			//std::ofstream outfile(name, std::ofstream::binary);
			//outfile.write((const char*)rgbaFrame.data, rgbaFrame.dataSize);
			//outfile.close();
			
			
			totalStartTime = std::chrono::steady_clock::now();
		}		
	}
				  
    return 0;
}