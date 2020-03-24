#pragma once

#include <Wizzy.h>

class Scene : public wz::Resource
{
	
public:
	__HANDLE_DEF;
	Scene(const wz::ResData& data, const wz::PropertyTable& props);
	Scene();
	virtual wz::ResData Serialize() const override;

	inline wz::ECSManager& GetEcs() { return m_ecs; }

	inline static bool IsFileBinary() { return true; }

private:
	wz::ECSManager m_ecs;
public:
	static wz::Resource* Create(const wz::ResData& data, const wz::PropertyTable& props);
	static const wz::PropertyTable& GetTemplateProps();
};
