#include "Entity.h"

void DerivedBehavior1::Update() {

	std::cout << m_name << "\tX_POS: " << ++owner->GetX() << "\tY_POS: " << ++owner->GetY() << '\n'; 
}

void DerivedBehavior2::Update() {

	std::cout << m_name << "\tX_POS: " << --owner->GetX() << "\tY_POS: " << --owner->GetY() << '\n';
}