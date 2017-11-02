#pragma once

#include "stdafx.h"

#include "drumClassify.h"
CommandLineOptions cmd_options;
string toy_withName;
int helpopt;
int usageopt;
int verboseopt;

//ref - toy_with_stereoMFCC line 3118 in mudbox.cpp
// and other examples in that file. (included @ bottom of this file)

DrumFeatureExtractor::DrumFeatureExtractor()
{
}

DrumFeatureExtractor::~DrumFeatureExtractor()
{
}

void DrumFeatureExtractor::extractDrumFeatures(string sfName1, bool single_vector/*, bool stereo*/)
{


	/*
	//not checking whether it's peaking or not. 
	timeLoopNet->addMarSystem(mng.create("PeakerAdaptive", "peak"));
	timeLoopNet->updControl("PeakerAdaptive/peak/mrs_natural/peakEnd", 512);
	timeLoopNet->updControl("PeakerAdaptive/peak/mrs_real/peakSpacing", 0.5);
	timeLoopNet->updControl("PeakerAdaptive/peak/mrs_real/peakStrength", 0.7);
	timeLoopNet->updControl("PeakerAdaptive/peak/mrs_natural/peakStart", 0);
	timeLoopNet->updControl("PeakerAdaptive/peak/mrs_natural/peakStrengthReset", 2);
	timeLoopNet->updControl("PeakerAdaptive/peak/mrs_real/peakDecay", 0.9);
	timeLoopNet->updControl("PeakerAdaptive/peak/mrs_real/peakGain", 0.5);
	*/

	/*
	select whether stereo or mono feat extr to be used

	 if (stereo_ == true)	MarSystemManager mng;

	  {
		MarSystem* stereoFeatures = mng.create("StereoFeatures", "stereoFeatures");
		selectFeatureSet(stereoFeatures);
		featureNetwork->addMarSystem(stereoFeatures);
	  }
	  else
	  {
		featureNetwork->addMarSystem(mng.create("Stereo2Mono", "m2s"));
		MarSystem* featExtractor = mng.create("TimbreFeatures", "featExtractor");
		selectFeatureSet(featExtractor);
		featureNetwork->addMarSystem(featExtractor);
	  }
  
	*/

	MarSystemManager mng;

	//ALL THE FEATURES TO BE EXTRACTED.
	MarSystem* spectimeFanout = mng.create("Fanout", "spectimeFanout");
	spectimeFanout->addMarSystem(mng.create("ZeroCrossings", "zcrs"));
	spectimeFanout->addMarSystem(mng.create("Rms", "rms"));

	MarSystem* theSpectralNet = mng.create("Series", "theSpectralNet");
	theSpectralNet->addMarSystem(mng.create("Windowing", "ham"));
	theSpectralNet->addMarSystem(mng.create("Spectrum", "spk"));
	theSpectralNet->addMarSystem(mng.create("PowerSpectrum", "pspk"));

	MarSystem* featureFanout = mng.create("Fanout", "featureFanout");
	featureFanout->addMarSystem(mng.create("Centroid", "centroid"));
	featureFanout->addMarSystem(mng.create("Rolloff", "rolloff"));
	featureFanout->addMarSystem(mng.create("MFCC", "mfcc"));
	featureFanout->addMarSystem(mng.create("Kurtosis", "kurtosis"));
	featureFanout->addMarSystem(mng.create("Skewness", "skewness"));
	featureFanout->addMarSystem(mng.create("SFM", "sfm"));
	featureFanout->addMarSystem(mng.create("SCF", "scf"));

	theSpectralNet->addMarSystem(featureFanout);
	spectimeFanout->addMarSystem(theSpectralNet);

	if (single_vector) {

		mrs_natural memSize = 1; //dont know if this is a thing.
		mrs_natural winSize = 512;//was originally an int
		//need to set the input sample rate and ouptut sample rate

		MarSystem* bextractNetwork = mng.create("Series", "bextractNetwork");
		MarSystem* featureNetwork = mng.create("Series", "featureNetwork");//nothing attached.
		MarSystem *src = mng.create("SoundFileSource", "src");
		
		featureNetwork->addMarSystem(src);
		featureNetwork->linkControl("mrs_bool/hasData", "SoundFileSource/src/mrs_bool/hasData");
		
		featureNetwork->addMarSystem(spectimeFanout); //ADD IT TO FEATURE NETWORK.

		//define accSize as ~30 seconds, and create Accumulator Marsystem
		mrs_natural accSize_ = 1290;
		MarSystem* acc = mng.create("Accumulator", "acc");
		acc->updControl("mrs_natural/maxTimes", accSize_);
		acc->updControl("mrs_string/mode", "explicitFlush");
		acc->updControl("mrs_natural/timesToKeep", 1);
		
		//intecronnecting (bextract (accumulator (featNetwork)))
		acc->addMarSystem(featureNetwork); //add feat net TO the accumulator ?!
		
		bextractNetwork->addMarSystem(acc);

		bextractNetwork->addMarSystem(featureNetwork);

		//access the control
		bextractNetwork->linkControl("Accumulator/acc/Series/featureNewtork/SoundFileSource/src/mrs_string/filename", "mrs_string/filename");
		bextractNetwork->linkControl("mrs_bool/hasData","Accumulator/acc/Series/featureNetwork/SoundFileSource/src/mrs_bool/hasData");
		bextractNetwork->linkControl("mrs_natural/pos",
			"Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_natural/pos");
		bextractNetwork->linkControl("mrs_real/duration",
			"Accumulator/acc/Series/featureNetwork/SoundFileSource/src/mrs_real/duration");
		bextractNetwork->linkControl("mrs_string/previouslyPlaying",
			"Accumulator/acc/Series/featureNetwork/SoundFileSource/src/mrs_string/previouslyPlaying");
		bextractNetwork->linkControl("mrs_string/currentlyPlaying",
			"Accumulator/acc/Series/featureNetwork/SoundFileSource/src/mrs_string/currentlyPlaying");
		//related to collections!
		/*bextractNetwork->linkControl("mrs_bool/currentCollectionNewFile",
			"Accumulator/acc/Series/featureNetwork/SoundFileSource/src/mrs_bool/currentCollectionNewFile");
		bextractNetwork->linkControl("Accumulator/acc/mrs_bool/flush",
			"Accumulator/acc/Series/featureNetwork/SoundFileSource/src/mrs_bool/currentCollectionNewFile");*/
		/* seems important to set reset, but these features not in my extractor.
		bextractNetwork->linkControl(
			"Accumulator/acc/Series/featureNetwork/TimbreFeatures/featExtractor/Series/spectralShape/STFT_features/spectrumFeatures/Flux/flux/mrs_bool/reset",
			"Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_bool/currentCollectionNewFile");*/
		/* Another example of resetting. - see documentation for if marsystem has reset property
			if (memSize != 0) {
			bextractNetwork->linkControl(
			"Accumulator/acc/Series/featureNetwork/TextureStats/tStats/mrs_bool/reset",
			"Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_bool/currentCollectionNewFile"
			);
		}
		*/
		//avoided labeling.

		//NECESSARY??? OR DUPLICATE??? I THINK THIS IS A DUPLICATE.
		// link controls to top-level to make life simpler
		/*bextractNetwork->linkControl("Series/featureNetwork/SoundFileSource/src/mrs_string/filename",
			"mrs_string/filename");
		bextractNetwork->linkControl("mrs_bool/hasData",
			"Series/featureNetwork/SoundFileSource/src/mrs_bool/hasData");
		bextractNetwork->linkControl("mrs_natural/pos",
			"Series/featureNetwork/SoundFileSource/src/mrs_natural/pos");
		bextractNetwork->linkControl("mrs_real/duration",
			"Series/featureNetwork/SoundFileSource/src/mrs_real/duration");
		*/
		//I feel like we've seen this before.
		/*bextractNetwork->linkControl("mrs_string/currentlyPlaying",
			"Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_string/currentlyPlaying");
		bextractNetwork->linkControl("mrs_bool/currentCollectionNewFile",
			"Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_bool/currentCollectionNewFile");*/

		//apparently you need to reset for new collection file?
		/*bextractNetwork->linkControl(
			"Series/featureNetwork/TimbreFeatures/featExtractor/Series/spectralShape/STFT_features/spectrumFeatures/Flux/flux/mrs_bool/reset",
			"Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_bool/currentCollectionNewFile");*/

		//added from my own code.
		bextractNetwork->updControl("mrs_string/filename", sfName1);
		
		//EXAMPLE CREATES ANNOTATOR - I DIDN'T.
		bextractNetwork->addMarSystem(mng.create("WekaSink", "wsink"));
		//UNSURE WHAT THIS IS
		bextractNetwork->updControl("WekaSink/wsink/mrs_bool/onlyStable", true);
		bextractNetwork->linkControl(
			"WekaSink/wsink/mrs_bool/resetStable",
			"Accumulator/acc/Series/featureNetwork/SoundFileSource/src/mrs_bool/startStable");
		bextractNetwork->linkControl("WekaSink/wsink/mrs_string/currentlyPlaying",
			"mrs_string/previouslyPlaying");
		bextractNetwork->updControl("WekaSink/wsink/mrs_string/filename", "singleVector.arff");

		//ignored collection stuff for now - went for main processing loop.
		//check out line 188 - doing downsample and updctrl for wekaSink.

		//MAIN PROCESSING LOOP
		MarControlPtr ctrl_hasData = bextractNetwork->getctrl("mrs_bool/hasData");
		MarControlPtr ctrl_currentlyPlaying = bextractNetwork->getctrl("mrs_string/currentlyPlaying");
		MarControlPtr ctrl_previouslyPlaying = bextractNetwork->getctrl("mrs_string/previouslyPlaying");
		MarControlPtr ctrl_currentCollectionNewFile = bextractNetwork->getctrl("mrs_bool/currentCollectionNewFile");

		//mrs_string previouslyPlaying, currentlyPlaying;

		//int n = 0;
		//int advance = 0;

		//vector<string> processedFiles;
		//map<string, realvec> processedFeatures;

		//bool seen;
		//realvec fvec;
		//int label;

		//see line 220 for collection.
		//looking at 2nd example which just ticks.

		while (ctrl_hasData->to<mrs_bool>())
		{
			bextractNetwork->tick();
			//currentlyPlaying = ctrl_currentlyPlaying->to<mrs_string>();
			//dont need a classifier.
			//bextractNetwork->updControl("mrs_natural/advance", 1);
			/*for collections - see line 285
			currentlyPlaying = ctrl_currentlyPlaying->to<mrs_string>();
			  if (ctrl_currentCollectionNewFile->to<mrs_bool>())
			  {
				// if (memSize != 0)
				// featureNetwork->updControl("TextureStats/tStats/mrs_bool/reset",  true);
				cout << "Processing: " << n << " - " << currentlyPlaying << endl;
				n++;
			  }
			
			*/
		}

	}


	/*WEKA FILE STUFF*/
	/*MarSystem* total = mng.create("Series", "total");
	total->addMarSystem(acc);
	total->updControl("Accumulator/acc/mrs_natural/nTimes", 1); //originally was 1000
	total->addMarSystem(mng.create("WekaSink", "wsink"));

	//total->updControl("WekaSink/wsink/mrs_natural/nLabels", 3); //don't really have labels
	//total->updControl("WekaSink/wsink/mrs_string/labelNames", "center,halfedge,rimshot");
	total->updControl("WekaSink/wsink/mrs_string/filename", "drumClassifyOutput.arff");
	total->updControl("mrs_natural/inSamples", 512); //How can we do -sv mode like bextract.cpp????
	*/

	/*SOME CONTROLS*/
	/*featureNetwork->updControl("mrs_natural/inSamples", 512);
	featureNetwork->updControl("mrs_natural/onSamples", 512);
	featureNetwork->updControl("mrs_real/israte", 44100.0);
	featureNetwork->updControl("mrs_real/osrate", 44100.0);
	*/
	
	/*NEED A LOOP THAT TICKS THE SYSTEM.*/

	cin.get();
}
