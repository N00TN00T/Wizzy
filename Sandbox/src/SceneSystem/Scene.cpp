#include "spch.h"

#include "Scene.h"

Scene::Scene(const wz::ResData& data, const wz::PropertyTable& props)
 : wz::Resource(props) 
{
	m_ecs.Load(data);
}

Scene::Scene()
	: wz::Resource(wz::PropertyTable())
{
}

wz::ResData Scene::Serialize() const
{
	return m_ecs.Save();
}

wz::Resource* Scene::Create(const wz::ResData& data, const wz::PropertyTable& props)
{
	return new Scene(data, props);
}

const wz::PropertyTable& Scene::GetTemplateProps()
{
	return wz::PropertyTable();
}
