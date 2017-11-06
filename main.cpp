
#include "stdafx.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "drumClassify.h"

int main()
{
	DrumFeatureExtractor* drumClassify = new DrumFeatureExtractor();

	drumClassify->extractDrumFeatures("fil.wav", true/*, false*/); //want single vector for mono

	//drumClassify->extractDrumFeatures("center.mf", true/*, false*/); //want single vector for mono

	std::cin.get();

	return 0;
}