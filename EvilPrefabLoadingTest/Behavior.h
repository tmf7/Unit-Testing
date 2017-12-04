#ifndef TEST_BEHAVIOR_H
#define TEST_BEHAVIOR_H

#include <string>
#include <iostream>

class Entity;

class Behavior {
public:

	virtual void								Start() {}
	virtual void								Update() {}
	virtual void								SetOwner(Entity * newOwner) { owner = newOwner; }
	virtual std::unique_ptr<Behavior>			GetCopy() const = 0;

	virtual const std::string &					Name() const = 0;

protected:

	Entity *					owner;
};

class DerivedBehavior1 : public Behavior {
public:

	virtual void								Start() override { m_name = "Add"; }
	virtual void								Update() override;
	virtual std::unique_ptr<Behavior>			GetCopy() const override { return std::make_unique<DerivedBehavior1>(*this); }

	virtual const std::string &					Name() const override { return m_name; }

private:

	std::string				m_name;
};

class DerivedBehavior2 : public Behavior {
public:

	virtual void								Start() override { m_name = "Subtract"; }
	virtual void								Update() override;
	virtual std::unique_ptr<Behavior>			GetCopy() const override { return std::make_unique<DerivedBehavior2>(*this); }

	virtual const std::string &					Name() const override { return m_name; }

private:

	std::string				m_name;
};

#endif /* TEST_BEHAVIOR_H */