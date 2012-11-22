//============================================================================
// Name        : ActionRecognition.cpp
// Author      : Sayyed Mohammad Hashemi
// Version     :
// Copyright   : 
// Description : Ansi-style
//============================================================================
#include "stdafx.h"
#include <iostream>
#include "BackgroundModel.h"
#include "VideoProccessor.h"

using namespace std;

int main() {
	cout << "!!!Running!!!" << endl; // prints !!!Hello World!!!
	BackgroundModel bModel;

//	bModel.buildAllBackgroundsModel();

	bModel.loadBackgroundsModel();
	VideoProccessor vp;
	vp.bModel=bModel;
	vp.run(CAMERA);


	return 0;
}
