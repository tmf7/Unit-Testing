#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <vector>

// minimize the vtable size overhead associated with overriding a ton of virtual functions in derived classes, 
// instead just specialize statically accessable methods.

// PROBLEM: cant specialize the same template twice
// SOLUTION: inherit, then "override" base template's non-virtual methods (block visibility, unless through ptr/ref)
template<class type>
class ResourceManager {
public:

	int								DontSpecialize() { return this->Specialize(); }
	virtual int						Specialize() = 0;
	virtual const char *			GetClassType() const = 0;

protected:

	std::vector<type>				resourceList;
};


class IntManager : public ResourceManager<int> {
public:
	// base Load calls child LoadAndGet ==> Base DontSpecialize calls child Specialize
	virtual int						Specialize() override;
	int								ExtensionFn(int param) { return param << 2; }
	virtual const char *			GetClassType() const override { return "IntManager"; }
};


class FloatManager : public ResourceManager<float> {
public:

	int								ExtensionFn(int param) { return param << 4; }
	virtual int						Specialize() override;
	virtual const char *			GetClassType() const override { return "FloatManager"; }
};

#endif /* RESOURCE_MANAGER_H */