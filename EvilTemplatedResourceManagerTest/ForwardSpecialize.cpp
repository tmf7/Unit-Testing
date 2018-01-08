#include "BaseTemplate.h"

int IntManager::Specialize() {
	resourceList.reserve(100);
	return resourceList.capacity();
}

int FloatManager::Specialize() {
	resourceList.reserve(200);
	return resourceList.capacity();
}