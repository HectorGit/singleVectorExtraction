#pragma once

#include "stdafx.h"

#include "drumClassify.h"
CommandLineOptions cmd_options;
string toy_withName;
int helpopt;
int usageopt;
int verboseopt;

DrumFeatureExtractor::DrumFeatureExtractor()
{
	//see mudbox - toy with orca
	sropt = 44100.0;
	copt = 1;
	length = 5;
	bufferSize = 1024; //vs 512

	cout << "realtime feat extract into a single vector" << endl;

	//peakerNet = mng.create("Series", "peakerNet");
	//peakerNet->addMarSystem(mng.create("AudioSource", "src"));
	//peakerNet->updControl("AudioSource/src/mrs_real/israte",sropt);
	//peakerNet->updControl("AudioSource/src/mrs_natural/inSamples", bufferSize); //origin 512
	//peakerNet->updControl("AudioSource/src/mrs_natural/onSamples", bufferSize);
	//peakerNet->updControl("AudioSource/src/mrs_real/osrate", sropt);
	//peakerNet->updControl("AudioSource/src/mrs_natural/nChannels", copt);

	//peakerNet->addMarSystem(mng.create("PeakerAdaptive", "peak"));
	//peakerNet->updControl("PeakerAdaptive/peak/mrs_natural/peakEnd", bufferSize); //origin 512
	//peakerNet->updControl("PeakerAdaptive/peak/mrs_real/peakSpacing", 0.5);
	//peakerNet->updControl("PeakerAdaptive/peak/mrs_real/peakStrength", 0.5);
	//peakerNet->updControl("PeakerAdaptive/peak/mrs_natural/peakStart", 0);
	//peakerNet->updControl("PeakerAdaptive/peak/mrs_natural/peakStrengthReset", 2);
	//peakerNet->updControl("PeakerAdaptive/peak/mrs_real/peakDecay", 0.9);
	//peakerNet->updControl("mrs_natural/inSamples", bufferSize); //origin 512

	//then the extractor net
	net = mng.create("Series", "net");
	net->updControl("mrs_real/israte", sropt);
	net->updControl("mrs_natural/inSamples", bufferSize); //origin 512

	net->addMarSystem(mng.create("AudioSource", "otherSrc"));

	net->updControl("AudioSource/otherSrc/mrs_natural/nChannels", copt);
	net->updControl("AudioSource/otherSrc/mrs_natural/inSamples", bufferSize); //origin 512
	net->updControl("AudioSource/otherSrc/mrs_natural/bufferSize", bufferSize); //origin 512
	net->updControl("AudioSource/otherSrc/mrs_real/israte", sropt);

	//print info out
	srate = net->getctrl("mrs_real/israte")->to<mrs_real>();
	nChannels = net->getctrl("AudioSource/otherSrc/mrs_natural/nChannels")->to<mrs_natural>();
	cout << "AudioSource srate =  " << srate << endl;
	cout << "AudioSource nChannels = " << nChannels << endl;
	inSamples = net->getctrl("mrs_natural/inSamples")->to<mrs_natural>();
	cout << "net inSamples = " << inSamples << endl;

	MarSystem* spectrumOperation = mng.create("Series", "spectrumOperation");
	spectrumOperation->addMarSystem(mng.create("Windowing", "ham"));
	spectrumOperation ->addMarSystem(mng.create("Spectrum", "spec"));
	spectrumOperation->addMarSystem(mng.create("PowerSpectrum", "pspk"));

	net->addMarSystem(spectrumOperation);

	MarSystem* featureFanout = mng.create("Fanout", "featureFanout");
	featureFanout->addMarSystem(mng.create("Centroid", "centroid"));
	featureFanout->addMarSystem(mng.create("Rolloff", "rolloff"));
	featureFanout->addMarSystem(mng.create("MFCC", "mfcc"));
	featureFanout->addMarSystem(mng.create("Kurtosis", "kurtosis"));
	featureFanout->addMarSystem(mng.create("Skewness", "skewness"));

	net->addMarSystem(featureFanout);
	// FOR SOME REASON SETTING UP THIS - might be unnecessary

	//MarSystem* acc = mng.create("Accumulator", "acc");
	//acc->addMarSystem(net);

	total = mng.create("Series", "total");
	//total->addMarSystem(acc);
	//total->updControl("Accumulator/acc/mrs_natural/nTimes", 20); //400 ?!!
	//total->addMarSystem(mng.create("Mean", "mn"));
	total->updControl("mrs_real/israte", 44100.0);
	total->updControl("mrs_natural/inSamples", bufferSize); //origin 512
	total->addMarSystem(net);

	/*out2.create(total->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
		total->getctrl("mrs_natural/onSamples")->to<mrs_natural>());*/
	
}

DrumFeatureExtractor::~DrumFeatureExtractor()
{
}

void DrumFeatureExtractor::extractDrumFeatures() {
	//peakerNet->updControl("AudioSource/src/mrs_real/israte", sropt);
	//peakerNet->updControl("AudioSource/src/mrs_natural/inSamples", bufferSize); //origin 512
	//peakerNet->updControl("AudioSource/src/mrs_natural/onSamples", bufferSize);
	//peakerNet->updControl("AudioSource/src/mrs_real/osrate", sropt);
	//peakerNet->updControl("AudioSource/src/mrs_natural/nChannels", copt);
	//peakerNet->updControl("AudioSource/src/mrs_bool/initAudio", true);
	
	net->updControl("AudioSource/otherSrc/mrs_natural/nChannels", copt);
	net->updControl("AudioSource/otherSrc/mrs_natural/inSamples", bufferSize); //origin 512
	net->updControl("AudioSource/otherSrc/mrs_natural/bufferSize", bufferSize); //origin 512
	net->updControl("AudioSource/otherSrc/mrs_real/israte", sropt);
	net->updControl("AudioSource/otherSrc/mrs_bool/initAudio", true);

	mrs_natural iterations = (mrs_natural)((srate*length) / inSamples);

	cout << "Iterations = " << iterations << endl;

	for (mrs_natural t = 0; t < iterations; t++)
	{
		//cout << "iteration: " << t << endl;
		//in1.create(peakerNet->getctrl("mrs_natural/inObservations")->to<mrs_natural>(),
		//	peakerNet->getctrl("mrs_natural/inSamples")->to<mrs_natural>());

		//out1.create(peakerNet->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
		//	peakerNet->getctrl("mrs_natural/onSamples")->to<mrs_natural>());
		//peakerNet->process(in1, out1);
		//for(mrs_natural i; i < bufferSize; i++){
		//	if (out1(i) > 0) {
				total->tick();
				const mrs_realvec& src_data =
					total->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
				cout << src_data << endl;
		//	}
		//}
	}
	delete total;
}

//void DrumFeatureExtractor::extractDrumFeatures()
//{
	//maybe should do a number of iterations
	//MarControlPtr ctrl_hasData = total->getctrl("AudioSource/otherSrc/mrs_bool/hasData");

	//mrs_natural iterations = (mrs_natural)((srate*length)/inSamples);

	//cout << "Iterations = " << iterations << endl;
	
	/*net->updControl("AudioSource/otherSrc/mrs_natural/nChannels", copt);
	net->updControl("AudioSource/otherSrc/mrs_natural/inSamples", bufferSize); //origin 512
	net->updControl("AudioSource/otherSrc/mrs_natural/bufferSize", bufferSize); //origin 512
	net->updControl("AudioSource/otherSrc/mrs_real/israte", sropt);
	net->updControl("AudioSource/otherSrc/mrs_bool/initAudio", true);*/

	//for (mrs_natural t = 0; t < iterations; t++)
	//{
	//	total->tick();
	//	const mrs_realvec& src_data =
	//		total->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
	//	cout << src_data << endl;
	//}

	//while (ctrl_hasData->to<mrs_bool>()) //do a number of iterations.
	//{
		//i don't think we'll get to play back audio the way this is set up.
		/*peakerNet->process(in1, out1);//hmm not sure if this will work with the parallel.
		for (int i = 0; i < bufferSize; i++) {
			//indexing of realvec is with parenthesis.
			if (out1(i) > 0) {
				//net->process(out1, out2);
				total->tick();
				const mrs_realvec& src_data =
					total->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
				cout << src_data << endl; 
			}
		}*/
		/*total->tick();
		const mrs_realvec& src_data =
			total->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
		cout << src_data << endl;
	}*/
	//cin.get();
//}