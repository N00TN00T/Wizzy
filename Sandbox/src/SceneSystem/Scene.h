#pragma once

#include <Wizzy.h>

class Scene : public wz::Resource
{
	
public:
	__HANDLE_DEF;
	Scene(const wz::ResData& data, const wz::PropertyLibrary& props);
	Scene();
	virtual wz::ResData Serialize() const override;

	inline wz::ECSManager& GetEcs() { return m_ecs; }

private:
	wz::ECSManager m_ecs;
public:
	static wz::Resource* Create(const wz::ResData& data, const wz::PropertyLibrary& props);
	static const wz::PropertyLibrary& GetTemplateProps();
};
