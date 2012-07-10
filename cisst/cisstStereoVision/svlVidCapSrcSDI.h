#ifndef _svlVidCapSrcSDI_h
#define _svlVidCapSrcSDI_h

//
// svlVidCapSrcSDI.h
//
// Copyright (C) 2007 NVIDIA Corporation
//
// This demo demonstrates how to send two different
// video streams from the Quadro FX SDI using
// NV_present_video.
//

#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glx.h>
#include <GL/glxext.h>
#include <GL/glu.h>

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>

#include <list>
#include <iostream>
#include <sstream>

#include <X11/keysym.h>
#include <X11/Xlib.h>

#include <getopt.h>

#include "NVCtrlLib.h"
#include "NVCtrl.h"
#include "NvSDIin.h"
#include "NvSDIutils.h"
#include "NvSDIout.h"
#include "fbo.h"
#include "GraphicsN.h"

#include "ANCapi.h"
#include "commandline.h"
#include "fbo.h"
#include "audio.h"
#include "ringbuffer.h"

#include <cuda_runtime.h>
#include <cuda_gl_interop.h>

#include <cisstStereoVision/svlFilterSourceVideoCapture.h>
#include <cisstStereoVision/svlRenderTargets.h>

#define MAX_VIDEO_OUT_STREAMS 2

class osaThread;
class svlBufferImage;
class svlVidCapSrcSDIThread;

// Render Target
class svlVidCapSrcSDIRenderTarget : public svlRenderTargetBase
{
    friend class svlRenderTargets;
    friend class svlVidCapSrcSDIThread;

public:
    bool SetImage(unsigned char* buffer, int offsetx, int offsety, bool vflip, int index=0);
    unsigned int GetWidth();
    unsigned int GetHeight();
    void* ThreadProc(void* CMN_UNUSED(param));
    void* CaptureThreadProc(void* param);

    GLenum  CaptureVideo(GLuint dropBool=0,float runTime = 0.0);
    GLboolean OutputVideo ();
    GLboolean DrawOutputScene(bool drawOverlay=false, GLuint cudaOutTexture1=-1, GLuint cudaOutTexture2=-1, unsigned char* vtkPixelData = new unsigned char[0]);
    GLenum DisplayVideo(bool drawFrameRate = false);
    void Shutdown();
    void MakeCurrentGLCtx(){glXMakeCurrent(dpy, win, ctx);};

protected:
    svlVidCapSrcSDIRenderTarget(unsigned int deviceID, unsigned int displayID=0);
    svlVidCapSrcSDIRenderTarget(Display * d, HGPUNV * g, unsigned int video_format, GLsizei num_streams, unsigned int deviceID=0, unsigned int displayID = 0);
    ~svlVidCapSrcSDIRenderTarget();

private:
    int SystemID;
    int DigitizerID;

    osaThread *Thread, *CaptureThread;
    osaThreadSignal NewFrameSignal;
    osaThreadSignal ThreadReadySignal;
    bool TransferSuccessful;
    bool KillThread;
    bool ThreadKilled;
    bool Running;

    Display * dpy;		// Display
    Window win;            // Window
    HGPUNV *gpu;
    GLsizei m_num_streams;
    unsigned char *m_overlayBuf[MAX_VIDEO_STREAMS];   // System memory buffers
    CFBO m_FBO[MAX_VIDEO_STREAMS];			// Channel 1 FBO
    OutputOptions outputOptions;
    CNvSDIout m_SDIout;
    GLuint m_OutTexture[MAX_VIDEO_STREAMS];
    CFBO gFBO[MAX_VIDEO_OUT_STREAMS];  // FBOS, need two, one per channel
    CaptureOptions m_captureOptions;
    bool m_SDIoutEnabled;

    //capture
    unsigned char *m_memBuf[MAX_VIDEO_STREAMS];   // System memory buffers
    CaptureOptions captureOptions;
    Colormap cmap;
    GLXContext ctx;        // OpenGL rendering context
    CNvSDIin m_SDIin;
    GLuint m_windowWidth;                   // Window width
    GLuint m_windowHeight;                  // Window height
    double m_inputFrameRate;
    GLuint gTexObjs[MAX_VIDEO_STREAMS];

    void drawVTKPixels(GLuint gWidth, GLuint gHeight, unsigned char* vtkPixelData);
    void drawCircle(GLuint gWidth, GLuint gHeight);

    void calcWindowSize();
    void drawOne();
    void drawTwo();
    void drawThree();
    void drawFour();
    GLuint getTextureFromBuffer(unsigned int index);

    //bool setupSDIDevices (Display * d, HGPUNV * g, unsigned int video_format, GLsizei num_streams);
    bool setupSDIDevices(Display *d=NULL,HGPUNV *g=NULL);
    bool setupSDIinDevice(Display *d,HGPUNV *g);
    bool setupSDIoutDevice(Display * d, HGPUNV * g, unsigned int video_format);
    GLboolean setupSDIGL();
    GLboolean setupSDIinGL();
    GLboolean setupSDIoutGL();

    bool startSDIPipeline();
    bool stopSDIPipeline();

    bool cleanupSDIDevices ();
    bool cleanupSDIinDevices();
    bool cleanupSDIoutDevices ();
    GLboolean cleanupSDIGL ();
    GLboolean cleanupSDIinGL();
    GLboolean cleanupSDIoutGL();
    bool destroyWindow();

    Window createWindow();

    bool translateImage(unsigned char* src, unsigned char* dest, const int width, const int height, const int trhoriz, const int trvert, bool vflip);
};

class svlVidCapSrcSDI : public svlVidCapSrcBase
{
    friend class svlVidCapSrcSDIRenderTarget;
    friend class svlVidCapSrcSDIThread;

    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    static svlVidCapSrcSDI* GetInstance();
    svlFilterSourceVideoCapture::PlatformType GetPlatformType();
    int SetStreamCount(unsigned int numofstreams);
    int GetStreamCount(void){return NumOfStreams;};
    int GetDeviceList(svlFilterSourceVideoCapture::DeviceInfo **deviceinfo);
    int Open();
    void Close();
    int Start();
    svlImageRGB* GetLatestFrame(bool waitfornew, unsigned int videoch = 0);
    int Stop();
    bool IsRunning();
    int SetDevice(int devid, int inid, unsigned int videoch = 0);
    int GetWidth(unsigned int videoch = 0);
    int GetHeight(unsigned int videoch = 0);

    int GetFormatList(unsigned int deviceid, svlFilterSourceVideoCapture::ImageFormat **formatlist);
    int GetFormat(svlFilterSourceVideoCapture::ImageFormat& format, unsigned int videoch = 0);
    void Release();

    bool IsCaptureSupported(unsigned int sysid, unsigned int digid = 0);
    bool IsOverlaySupported(unsigned int sysid, unsigned int digid = 0);

    svlVidCapSrcSDIThread* GetCaptureProc(int i){return CaptureProc[i];};

private:

    svlVidCapSrcSDI();
    ~svlVidCapSrcSDI();

    //TODO::Match with sdi in
    unsigned int NumOfStreams;
    bool InitializedInput, InitializedOutput;
    bool Running;

    vctDynamicVector<int> SystemID;
    vctDynamicVector<int> DigitizerID;
    vctDynamicVector<svlBufferImage*> ImageBuffer;

    svlVidCapSrcSDIThread** CaptureProc;
    osaThread** CaptureThread;

};

class svlVidCapSrcSDIThread
{
public:
    svlVidCapSrcSDIThread(int streamid);
    ~svlVidCapSrcSDIThread() {Shutdown();XCloseDisplay(dpy);};
    void* Proc(svlVidCapSrcSDI* baseref);

    bool WaitForInit() { InitEvent.Wait(); return InitSuccess; }
    bool IsError() { return Error; }

    bool SetupSDIDevices(Display *d=NULL,HGPUNV *g=NULL);
    GLboolean  SetupGL();

    bool StartSDIPipeline();
    bool StopSDIPipeline();
    GLenum DisplayVideo(bool drawFrameRate);
    GLenum  CaptureVideo(GLuint dropBool=0,float runTime = 0.0);
    Window CreateWindow();
    CNvSDIin GetSDIin(){return m_SDIin;};
    void Shutdown();
    void MakeCurrentGLCtx();
    Display * GetDisplay(){return dpy;};
    HGPUNV * GetGPU() {return gpu;};

private:
    int StreamID;
    bool Error;
    osaThreadSignal InitEvent;
    bool InitSuccess;
    IplImage *Frame;

    unsigned char *m_memBuf[MAX_VIDEO_STREAMS];   // System memory buffers
    CaptureOptions captureOptions;
    //X stuff
    Display *dpy;          // Display
    Window win;            // Window
    Colormap cmap;
    GLXContext ctx;        // OpenGL rendering context
    HGPUNV *gpu;
    CNvSDIin m_SDIin;
    GLuint m_windowWidth;                   // Window width
    GLuint m_windowHeight;                  // Window height
    double m_inputFrameRate;
    GLuint gTexObjs[MAX_VIDEO_STREAMS];

    bool setupSDIinGL();
    bool setupSDIinDevice(Display *d,HGPUNV *g);

    void calcWindowSize();
    void drawOne();
    void drawTwo();
    void drawCircle(GLuint gWidth, GLuint gHeight);
    void drawThree();
    void drawFour();
    GLuint getTextureFromBuffer(unsigned int index);

    GLboolean cleanupGL();
    bool cleanupSDIin();
    bool cleanupSDIDevices();
    bool destroyWindow();

};

CMN_DECLARE_SERVICES_INSTANTIATION(svlVidCapSrcSDI)

#endif // _svlVidCapSrcSDI_h
