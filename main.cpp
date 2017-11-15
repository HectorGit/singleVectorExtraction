
#include "stdafx.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "drumClassify.h"

int main()
{
	DrumFeatureExtractor* drumClassify = new DrumFeatureExtractor();

	drumClassify->extractDrumFeatures();

	std::cin.get();

	return 0;
}