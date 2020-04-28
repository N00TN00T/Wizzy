#pragma once

#include <Wizzy.h>


class Sandbox
	: public wz::Application
{
public:

	Sandbox();

	~Sandbox();

	virtual void Init() override;
	
	virtual void Shutdown() override;

};