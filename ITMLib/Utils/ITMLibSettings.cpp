// Copyright 2014-2017 Oxford University Innovation Limited and the authors of InfiniTAM

#include "ITMLibSettings.h"
using namespace ITMLib;
using namespace ORUtils;

#include <climits>
#include <cmath>

ITMLibSettings::ITMLibSettings(void)
:	sceneParams(0.04f, 100, 0.004f, 0.3f, 2.5f, true),/*
	width of the band of the TSD function
	observations per voxel are averaged, after that sliding average
	voxel size
	min integration depth
	max integration depth
	stopping integration when maxW is reached? */
	surfelSceneParams(0.5f, 0.6f, static_cast<float>(20 * M_PI / 180), 0.01f, 0.004f, 3.5f, 25.0f, 4, 1.0f, 5.0f, 20, 10000000, true, true)
	/*
	deltaRadius_                  The maximum fraction by which a new surfel can have a larger radius than the surfel into which it is being fused if full fusion is to occur.
    gaussianConfidenceSigma_      The sigma value for the Gaussian used when calculating the sample confidence.
    maxMergeAngle_                The maximum angle allowed between the normals of a pair of surfels if they are to be merged.
    maxMergeDist_                 The maximum distance allowed between a pair of surfels if they are to be merged.
    maxSurfelRadius_              The maximum radius a surfel is allowed to have.
    minRadiusOverlapFactor_       The minimum factor by which the radii of a pair of surfels must overlap if they are to be merged.
    stableSurfelConfidence_       The confidence value a surfel must have in order for it to be considered "stable".
    supersamplingFactor_          The factor by which to supersample (in each axis) the index image used for finding surfel correspondences.
    trackingSurfelMaxDepth_       The maximum depth a surfel must have in order for it to be used for tracking.
    trackingSurfelMinConfidence_  The minimum confidence value a surfel must have in order for it to be used for tracking.
    unstableSurfelPeriod_         The number of time steps a surfel is allowed to be unstable without being updated before being removed.
    unstableSurfelZOffset_        The z offset to apply to unstable surfels when trying to ensure that they are only rendered if there is no stable alternative.
    useGaussianSampleConfidence_  Whether or not to use a Gaussian-weighted sample confidence as described in the Keller paper.
    useSurfelMerging_             Whether or not to use surfel merging.
    */
{
	// skips every other point when using the colour renderer for creating a point cloud
	skipPoints = true;

	// create all the things required for marching cubes and mesh extraction
	// - uses additional memory (lots!)
	createMeshingEngine = true;

#ifndef COMPILE_WITHOUT_CUDA
	deviceType = DEVICE_CUDA;
#else
#ifdef COMPILE_WITH_METAL
	deviceType = DEVICE_METAL;
#else
	deviceType = DEVICE_CPU;
#endif
#endif

	//deviceType = DEVICE_CPU;

	/// how swapping works: disabled, fully enabled (still with dragons) and delete what's not visible - not supported in loop closure version
	swappingMode = SWAPPINGMODE_DISABLED;

	/// enables or disables approximate raycast
	useApproximateRaycast = false;

	/// enable or disable bilateral depth filtering
	useBilateralFilter = false;

	/// what to do on tracker failure: ignore, relocalise or stop integration - not supported in loop closure version
	behaviourOnFailure = FAILUREMODE_IGNORE;

	/// switch between various library modes - basic, with loop closure, etc.
	libMode = LIBMODE_BASIC;
	//libMode = LIBMODE_BASIC_SURFELS;

	// Default ICP tracking
	/*trackerConfig = "type=icp,levels=rrrbb,minstep=1e-3,"
					"outlierC=0.01,outlierF=0.002,"
					"numiterC=10,numiterF=2,failureDec=5.0"; // 5 for normal, 20 for loop closure
*/
	// Depth-only extended tracker:
	/* trackerConfig = "type=extended,levels=rrbb,useDepth=1,minstep=1e-4,"
					  "outlierSpaceC=0.1,outlierSpaceF=0.004,"
					  "numiterC=20,numiterF=50,tukeyCutOff=8,"
					  "framesToSkip=20,framesToWeight=50,failureDec=20.0"; */
 
	// For hybrid intensity+depth tracking:
	trackerConfig = "type=extended,levels=bbb,useDepth=1,useColour=1,"
					  "colourWeight=0.3,minstep=1e-4,"
					  "outlierColourC=0.175,outlierColourF=0.005,"
					  "outlierSpaceC=0.1,outlierSpaceF=0.004,"
					  "numiterC=20,numiterF=50,tukeyCutOff=8,"
					  "framesToSkip=20,framesToWeight=50,failureDec=20.0"; //"framesToSkip=20,framesToWeight=50,failureDec=20.0";
 
	// Colour only tracking, using rendered colours
	//trackerConfig = "type=rgb,levels=rrbb";

	//trackerConfig = "type=imuicp,levels=tb,minstep=1e-3,outlierC=0.01,outlierF=0.005,numiterC=4,numiterF=2";
	//trackerConfig = "type=extendedimu,levels=ttb,minstep=5e-4,outlierSpaceC=0.1,outlierSpaceF=0.004,numiterC=20,numiterF=5,tukeyCutOff=8,framesToSkip=20,framesToWeight=50,failureDec=20.0";

	// Surfel tracking
	if(libMode == LIBMODE_BASIC_SURFELS)
	{
		trackerConfig = "extended,levels=rrbb,minstep=1e-4,outlierSpaceC=0.1,outlierSpaceF=0.004,numiterC=20,numiterF=20,tukeyCutOff=8,framesToSkip=0,framesToWeight=1,failureDec=20.0";
	}
}

MemoryDeviceType ITMLibSettings::GetMemoryType() const
{
	return deviceType == DEVICE_CUDA ? MEMORYDEVICE_CUDA : MEMORYDEVICE_CPU;
}
