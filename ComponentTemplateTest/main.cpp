#include <iostream>
#include "Classes.h"

#define MORE_CODE 0

int main( int argc, const char * argv ) {

	using namespace rtti;
	
	GameObject test;

	// AddComponent test
	test.AddComponent< Component >( "Component" );
	test.AddComponent< Collider >( "Collider" );
	test.AddComponent< BoxCollider >( "BoxCollider_A" );
	test.AddComponent< BoxCollider >( "BoxCollider_B" );

#if MORE_CODE
	test.AddComponent< RenderImage >( "RenderImage" );
#endif

	std::cout << "Added:\n------\nComponent\t(1)\nCollider\t(1)\nBoxCollider\t(2)\nRenderImage\t(0)\n\n";

	// GetComponent test
	auto & componentRef		= test.GetComponent< Component >();
	auto & colliderRef		= test.GetComponent< Collider >();
	auto & boxColliderRef1	= test.GetComponent< BoxCollider >();
	auto & boxColliderRef2	= test.GetComponent< BoxCollider >();		// boxColliderB == boxColliderA here because GetComponent only gets the first match in the class hierarchy
	auto & renderImageRef	= test.GetComponent< RenderImage >();		// should be &nullptr with MORE_CODE 0

	std::cout << "Values:\n-------\ncomponentRef:\t\t"	<< componentRef.value
			  << "\ncolliderRef:\t\t"					<< colliderRef.value	
			  << "\nboxColliderRef1:\t"					<< boxColliderRef1.value
			  << "\nboxColliderRef2:\t"					<< boxColliderRef2.value
			  << "\nrenderImageRef:\t\t"				<< ( &renderImageRef != nullptr ? renderImageRef.value : "nullptr" );

	// GetComponents test
	auto allColliders = test.GetComponents< Collider >();
	std::cout << "\n\nThere are (" << allColliders.size() << ") collider components attached to the test GameObject:\n";
	for ( auto && c : allColliders ) {
		std::cout << c->value << '\n';
	}

	// RemoveComponent test
	test.RemoveComponent< BoxCollider >();								// removes boxColliderA
	auto & boxColliderRef3		= test.GetComponent< BoxCollider >();	// now this should be the second BoxCollider "BoxCollider_B"

	std::cout << "\n\nFirst BoxCollider instance removed\nboxColliderRef3:\t" << boxColliderRef3.value << '\n';

#if MORE_CODE
	// RemoveComponent return test
	int removed = 0;
	while ( test.RemoveComponent< Component >() ) {
		++removed;
	}
#else
	// RemoveComponents test
	int removed = test.RemoveComponents< Component >();
#endif

	std::cout << "\nSuccessfully removed (" << removed << ") components from the test GameObject\n";

	system( "PAUSE" );
	return 0;
}