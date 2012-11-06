#include "StdAfx.h"
#include "AoiAndProcessorContainer.h"


AoiAndProcessorContainer::AoiAndProcessorContainer(void)
{
	count_ = 0;
}


AoiAndProcessorContainer::~AoiAndProcessorContainer(void)
{
}

bool AoiAndProcessorContainer::addAoi(AreaOfInterest* a) {
	for (unsigned i=0; i<aois_references_.size(); i++) {
		if (aois_references_[i] == a)
			return false;
	}
	aois_references_.push_back(a);
	count_++;
}