#include "../../Include/DebugInscriber.hpp"
#include <iostream>  
#include <fstream>  
DebugInscriber DebugInscriber::mInstance;

DebugInscriber::DebugInscriber() :mPassTime(0), mPassCount(0), 
mAllTime(0.0f),mAllFrameCount(0),mAllVertices(0),mAllIndices(0), mAllPolygon(0), mAllCallBatch(0),
mPolygon(0),mVertices(0),mIndices(0), mCallBatch(0),mUpdateTime(1.0f),mFpsAverage(0.f), mFrameCost(0.f)
{
}


DebugInscriber::~DebugInscriber()
{
}


void DebugInscriber::BeginInscriber(float tick)
{
	mPassTime += tick;
    mPassCount++;
    mAllTime += tick;
    mAllFrameCount++;
	if (mPassTime > mUpdateTime)
	{
		mCallBatchPerSecond = static_cast<float>(mCallBatch) / mPassTime;
        mRenderIndicesPerSecond = static_cast<float>(mIndices) / mPassTime;
        mRenderVerticesPerSecond = static_cast<float>(mVertices) / mPassTime;
        mRenderPolyonPerSecond = static_cast<float>(mPolygon) / mPassTime;

        mCallBatchPerFrame = static_cast<float>(mCallBatch) / mPassCount;
        mRenderIndicesPerFrame = static_cast<float>(mIndices) / mPassCount;
        mRenderVerticesPerFrame = static_cast<float>(mVertices) / mPassCount;
        mRenderPolyonPerFrame = static_cast<float>(mPolygon) / mPassCount;
		mFpsAverage = mPassCount / static_cast<float>(mPassTime);
		mFrameCost = mPassTime * 1000 / static_cast<float>(mPassCount);
		mCallBatch = 0;
		mPassCount = 0;
		mPassTime = 0.f;
        mVertices = 0;
        mIndices = 0;
        mPolygon = 0;
	}
}

void DebugInscriber::EndInscriber()
{
}

DebugInscriber * DebugInscriber::GetInstance()
{
	return &mInstance;
}

float DebugInscriber::GetAverageCallBatch()
{
    return mAllCallBatch/mAllTime;
}

float DebugInscriber::GetAverageFPS()
{
	return mFpsAverage;
}

float DebugInscriber::GetFrameCost()
{
	return mFrameCost;
}

float DebugInscriber::GetFPSPerSecond()
{
    return mPassCount/mPassTime;
}


float DebugInscriber::GetAverageVerticesRenderCount()
{
    return mAllVertices / mAllTime;
}


float DebugInscriber::GetAverageIndicesRenderCount()
{
    return static_cast<float>(static_cast<double>(mAllIndices) / mAllTime);
}
