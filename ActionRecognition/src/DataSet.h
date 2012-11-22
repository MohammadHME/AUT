/*
 * DataSet.h
 *
 *  Created on: Nov 21, 2012
 *      Author: mohammad
 */
#include <string.h>
#ifndef DATASET_H_
#define DATASET_H_

class DataSet {
public:
	DataSet();
	virtual ~DataSet();
	std::string actors[];
	std::string actions[];
};

#endif /* DATASET_H_ */
