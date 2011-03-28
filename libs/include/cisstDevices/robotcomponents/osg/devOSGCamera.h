

#ifndef _devOSGCamera_h
#define _devOSGCamera_h

#include <osgViewer/Viewer>

#include <cisstVector/vctDynamicMatrix.h>
#include <cisstVector/vctFrame4x4.h>
#include <cisstDevices/robotcomponents/osg/devOSGBody.h>
#include <cisstDevices/robotcomponents/osg/devOSGWorld.h>

#include <cisstMultiTask/mtsTaskContinuous.h>

#include <cisstDevices/devConfig.h>
#if CISST_DEV_HAS_OPENCV22
#include <opencv2/opencv.hpp>
#endif

#include <cisstDevices/devExport.h>

class CISST_EXPORT devOSGCamera : 

  public mtsTaskContinuous,
  public osgViewer::Viewer{

 protected:

  //! User data for the camera
  /** 
      This class stores a pointer to a camera object. This pointer is is used
      during traversals to capture/process images and update the
      orientation/position of the camera.
  */
  class Data : public osg::Referenced {
  private:
    //! Pointer to a camera object
    osg::ref_ptr<devOSGCamera> camera;
  public:
    //! Default constructor.
    Data( devOSGCamera* camera ) : camera( camera ){}
    //! Get the pointer to the camera
    devOSGCamera* GetCamera() { return camera; }
  };
  


  //! Update Callback
  /**
     This callback is used to update the position/orientation of the camera
     during the update traversal.     
  */
  class UpdateCallback : public osg::NodeCallback {
    //! Callback operator
    /**
       This operator is called during the update traversal. It's purpose is to
       update the position/orientation of the camera by calling the Update
       method.
     */
    void operator()( osg::Node* node, osg::NodeVisitor* );
  };


  //! Update the position and orientation of the camera
  /**
     This method can be called to update the position and orientation of
     the camera. It is called from the update callback during the update 
     traversal.
  */
  void Update();

  //! Set the camera position and orientation
  /**
     This method sets the position and orientation of the OSG camera.
     \param Rt A 4x4 frame providing the position and orientation
  */
  void SetMatrix( const vctFrame4x4<double>& Rt );

  //! Get the transformation
  /**
     This MTS function reads the position and orientation of the camera.
     It is called by the Update method during the update traversal.
  */
  mtsFunctionRead ReadTransformation;

  // Only enable this if OpenCV2 is enabled
#if CISST_DEV_HAS_OPENCV22

  //! Final drawing callback
  /**
     This class is used to capture/process images during the final drawing
     callback traversal.
  */
  class FinalDrawCallback : public osg::Camera::DrawCallback {
    
  public:

    // Data for the final draw callback. This data is used to copy images
    // outside the drawing traversal.
    class Data : public osg::Referenced {

    private:
      
      bool visibilityrequest;
      bool rangerequest;
      bool depthrequest;
      bool colorrequest;

      //! Converted 3D range data.
      /**
        Create a 3xN range data matrix.
	[ x1 ... xN ]
	[ y1 ... yN ]
	[ z1 ... zN ]
      */
      vctDynamicMatrix<double> rangedata;

      //! Occlusion image.
      /**
	 At each image point [x,y] you get a list of pointer to bodies that 
	 are sorted by visibility
      */
      vctDynamicMatrix< std::list< devOSGBody* > > visibilityimage;
      
      //! Depth image
      /**
	 This is a depth image, where at each pixel, you have the depth of 
	 the projected point.
      */
      cv::Mat depthimage;
      
      //! RGB image
      cv::Mat rgbimage;
      
    public:
      
      Data( size_t width, size_t height );
      ~Data();
      
      void RequestVisibilityImage() { visibilityrequest = true; }
      void RequestRangeData()       { rangerequest = true; }
      void RequestDepthImage()      { depthrequest = true; }
      void RequestRGBImage()        { colorrequest = true; }

      bool VisibilityImageRequested() const { return visibilityrequest; }
      bool RangeDataRequested()       const { return rangerequest; }
      bool DepthImageRequested()      const { return depthrequest; }
      bool RGBImageRequested()        const { return colorrequest; }

      vctDynamicMatrix< std::list<devOSGBody*> > GetVisibilityImage() const;
      vctDynamicMatrix<double> GetRangeData() const;
      cv::Mat GetDepthImage() const;
      cv::Mat GetRGBImage() const;

      void SetVisibilityImage(const vctDynamicMatrix< std::list<devOSGBody*> >& v);
      void SetRangeData( const vctDynamicMatrix<double>& rangedata );
      void SetDepthImage( const cv::Mat& depthimage );
      void SetRGBImage( const cv::Mat& rgbimage );

    };

  private:

    //! OSG image containing the depth buffer
    osg::ref_ptr<osg::Image> depthbufferimg;

    //! OSG image containing the color buffer
    osg::ref_ptr<osg::Image> colorbufferimg;
  
    //! Callback operator
    /**
       This callback is called during the final drawing traversal. This 
       operator calls the Capture method to grab frames from the depth and 
       color buffers.
    */
    virtual void operator () ( osg::RenderInfo& ) const;
    
    //! Convert the depth buffer to range data
    void ComputeRangeData( osg::Camera* camera ) const;

    //! Convert the depth buffer to range data
    void ComputeVisibilityImage( osg::Camera* camera ) const;

    //! Convert the depth buffer to a depth image
    void ComputeDepthImage( osg::Camera* camera ) const;

    //! Convert the color buffer to a color image
    void ComputeRGBImage( osg::Camera* camera ) const;

  public:
    
    //! Default constructor
    /**
       Constructor for a final drawing callback object. This object will
       determine the operation performed at each frame rendering.
       \param camera A pointer to an OSG camera
       \param capturedepth Read and convert depth buffer
       \param capturecolor Read and convert color buffer
    */
    FinalDrawCallback( osg::Camera* camera );
    
    ~FinalDrawCallback();
    
  };


#endif // CISST_DEV_HAS_OPENCV22

 public : 

  //! Create an OSG viewer (actually it's a OSG viewer)
  /**
     Create an OSG viewer wrapped in a MTS continuous task. The camera also
     creates an MTS required interface called "Transformation" if a function
     name is provided. This function is used to update the position of the
     camera at each update traversal.
     \param name The name of the camera
     \param world The world the camera belongs to
     \param x The X offset of the camera window
     \param y The Y offset of the camera window
     \param width The width of the camera image
     \param height The height of the camera image
     \param fovy The field of view angle
     \param aspectRatio The aspect ratio of the camera
     \param zNear The near buffer distance
     \param zFar  The far buffer distance
     \param fnname The name of a MTS read command the body will connect
     \param trackball Create the default trackball
  */
  devOSGCamera( const std::string& name,
		devOSGWorld* world,
		const std::string& fnname = "",
		bool trackball = true );

  ~devOSGCamera();

  void setCullMask( osg::Node::NodeMask mask )
  { getCamera()->setCullMask( mask ); }

  void Configure( const std::string& = "" ){}
  void Startup(){}
  void Run();
  void Cleanup(){}

  
};

#endif
