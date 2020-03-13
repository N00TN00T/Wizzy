#include "spch.h"

#include "Scene.h"

Scene::Scene(const wz::ResData& data, const wz::PropertyLibrary& props)
 : wz::Resource(props) 
{
	m_ecs.Load(data);
}

Scene::Scene()
	: wz::Resource(wz::PropertyLibrary())
{
}

wz::ResData Scene::Serialize() const
{
	return m_ecs.Save();
}

wz::Resource* Scene::Create(const wz::ResData& data, const wz::PropertyLibrary& props)
{
	return new Scene(data, props);
}

const wz::PropertyLibrary& Scene::GetTemplateProps()
{
	return wz::PropertyLibrary();
}
