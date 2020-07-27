#ifndef _H264_DECODER_H_
#define _H264_DECODER_H_

#include <iostream>
#include <assert.h>
#include <fstream>

// Media Foundation 
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mferror.h>

#include <combaseapi.h>
#include <mftransform.h>
#include <codecapi.h>
#include <mfidl.h>
#include <wmcodecdsp.h>
#include <chrono>
#include <assert.h>
#include <Objbase.h>
#include <wmcodecdsp.h>

#define H264_INBUF_SIZE 16384  /* number of bytes we read per chunk */


struct RawH264Frame {
	unsigned long long deviceTimestamp;
	double timestamp;
	unsigned char *data;
	int dataSize;
	int frameNumber;
};

struct RGBAFrame {
	unsigned long long deviceTimestamp;
	double timestamp;
	unsigned char *data;
	int dataSize;
	int frameNumber;
};


class H264Decoder {
public:
	H264Decoder(UINT32 width, UINT32 height);
	virtual ~H264Decoder();

	bool decodeFrame(RawH264Frame* rawH264Frame, RGBAFrame* rgbaFrame);

private:
	UINT32        m_width;
	UINT32        m_height;
	IMFTransform* m_pDecoder;
	IMFSample*    m_pDecoderInputSample;
	IMFMediaBuffer* m_pDecoderInputMediaBuffer;
	IMFSample*    m_pDecoderOutputSample;
	MFT_OUTPUT_DATA_BUFFER m_DecoderOutputBuffer;
	IMFTransform* m_pColorDsp;
	IMFSample* m_pDspInputSample;
	IMFSample* m_pDspOutputSample;
	IMFMediaBuffer* m_pDspOutMediaBuffer;
	MFT_OUTPUT_DATA_BUFFER m_DspOutputBuffer;
};

#endif