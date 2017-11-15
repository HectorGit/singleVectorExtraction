#pragma once
/*
==============================================================================

CentroidExtractor.h

==============================================================================
*/

#pragma once
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <iostream>
#include <math.h> //or cmath?

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include <cstdio>
#include <cstdlib>
#include <string>
#include <iomanip>

#include <marsyas/common_source.h>
#include <marsyas/Collection.h>
#include <marsyas/NumericLib.h>
#include <marsyas/CommandLineOptions.h>
#include <marsyas/FileName.h>
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/marsystems/MarSystemTemplateBasic.h>
#include <marsyas/marsystems/MarSystemTemplateAdvanced.h>
#include <marsyas/sched/EvValUpd.h>

#include <marsyas/marsystems/Spectrum2ACMChroma.h>

#include <time.h>
#include <marsyas/types.h>

#ifdef MARSYAS_MIDIIO
#include <RtMidi.h>
#endif

#ifdef MARSYAS_AUDIOIO
#include <RtAudio.h>
#endif

#ifdef MARSYAS_PNG
#include <pngwriter.h>
#endif


using namespace std;
using namespace Marsyas;

class DrumFeatureExtractor {

public:
	DrumFeatureExtractor();
	~DrumFeatureExtractor();																											//void feedForwardSingleInstance(/*Eigen::MatrixXf instance*/); //prints whether an instance was correctly classified
	void extractDrumFeatures();																						//and what classification it was assigned
private:
	MarSystemManager mng;
	MarSystem* peakerNet;
	MarSystem* net;
	MarSystem* total;
	string centerCollection;
	string halfEdgeCollection;
	string rimshotCollection;
	realvec in1;
	realvec out1;
	realvec out2;
	int bufferSize;
	mrs_natural copt;
	mrs_real sropt;
	mrs_real length;
	mrs_real srate;
	mrs_natural nChannels;
	mrs_natural inSamples;
};
