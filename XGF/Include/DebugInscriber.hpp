#pragma once
//#define XGF_DEBUG_USE_INSCRIBER

class DebugInscriber
{
public:
	DebugInscriber();
	~DebugInscriber();
    static DebugInscriber * GetInstance();

	void BeginInscriber(float ticktime);
	void EndInscriber();
    void CallABatch(int Indices, int Vertices) {
        mCallBatch++;
        mVertices += Vertices;
        mIndices += Indices; 
        mAllCallBatch++;
        mAllVertices += Vertices;
        mAllIndices += Indices;
    }
    void CallAPolygon() {
        mPolygon++;
        mAllPolygon++;
    }
    void SetUpdateTime(float time) { mUpdateTime = time; }
    float GetAverageCallBatch();
    float GetCallBatchPerSecond() { return mCallBatchPerSecond; }
    float GetCallBatchPerFrame() { return mCallBatchPerFrame; }
	//获得在每mUpdateTime时间内平均fps
    float GetAverageFPS();
    float GetFPSPerSecond();
    float GetVerticesRenderCountPerSecond() { return mRenderVerticesPerSecond; }
    float GetVerticesRenderCountPerFrame() { return mRenderVerticesPerFrame; }
    float GetAverageVerticesRenderCount();
    float GetIndicesRenderCountPerSecond() { return mRenderIndicesPerSecond; }
    float GetIndicesRenderCountPerFrame() { return mRenderIndicesPerFrame; }
    float GetAverageIndicesRenderCount();
    //float GetTriangleRenderCountPerSecond();
    float GetPolygonRenderCountPerSecond() { return mRenderPolyonPerSecond; }
    float GetPolygonRenderCountPerFrame() { return mRenderPolyonPerFrame; }

private:
	float mPassTime;
	int mPassCount;

    double mAllTime;
    int mAllFrameCount;

	static DebugInscriber mInstance;

	float mCallBatchPerSecond;
    float mRenderVerticesPerSecond;
    float mRenderIndicesPerSecond;
    float mRenderPolyonPerSecond;

    float mCallBatchPerFrame;
    float mRenderVerticesPerFrame;
    float mRenderIndicesPerFrame;
    float mRenderPolyonPerFrame;
	float mFpsAverage;

    int mAllCallBatch;
    int mAllVertices;
    int mAllIndices;
    int mAllPolygon;

    int mCallBatch;
    int mVertices;
    int mIndices;
    int mPolygon;

    float mUpdateTime;
};
#ifdef XGF_DEBUG_USE_INSCRIBER
#ifdef _DEBUG
#define DebugInscriber_CallABatch(Indices,Vertices) DebugInscriber::GetInstance()->CallABatch(Indices,Vertices);
#define DebugInscriber_Begin(p) DebugInscriber::GetInstance()->BeginInscriber(p);
#define DebugInscriber_End() DebugInscriber::GetInstance()->EndInscriber();
#define DebugInscriber_CallAPolygon() DebugInscriber::GetInstance()->CallAPolygon();

#else
#define DebugInscriber_CallABatch() ;
#define DebugInscriber_Begin(p) ;
#define DebugInscriber_End() ;
#define DebugInscriber_CallAPolygon() ;
#endif
#else
#define DebugInscriber_CallABatch(Indices,Vertices) DebugInscriber::GetInstance()->CallABatch(Indices,Vertices);
#define DebugInscriber_Begin(p) DebugInscriber::GetInstance()->BeginInscriber(p);
#define DebugInscriber_End() DebugInscriber::GetInstance()->EndInscriber();
#define DebugInscriber_CallAPolygon() DebugInscriber::GetInstance()->CallAPolygon();
#endif


