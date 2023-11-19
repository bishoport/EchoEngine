#include "ScriptableGameObjectManager.h"
#include <iostream>

#include "ECS/ScriptComponent.h"


namespace Scripting
{
	//-DLL------------------------------------------------------------------------
	ScriptableGameObjectManager::ScriptableGameObjectManager() {}
	void ScriptableGameObjectManager::LoadDll()
	{
		//Current directory as mono dir
		mono_set_dirs("C:/Program Files/Mono/lib", "C:/Program Files/Mono/etc");

		//Create mono domain
		m_ptrMonoDomain = mono_jit_init("CsEngine");
		//mono_domain_free();
		if (m_ptrMonoDomain)
		{
			//Load a mono assembly CsEngineScript.dll
			m_ptrCsEngineAssembly = mono_domain_assembly_open(m_ptrMonoDomain, "C:/Users/pdortegon/Documents/PROPIOS/EchoEngine/x64/Debug/CsEngineScript.dll");
			if (m_ptrCsEngineAssembly)
			{
				//Loading mono image
				m_ptrCsEngineAssemblyImage = mono_assembly_get_image(m_ptrCsEngineAssembly);
				if (m_ptrCsEngineAssemblyImage)
				{
					std::cout << "Mono DLL Load Success" << std::endl;
				}
			}
		}
	}
	ScriptableGameObjectManager::~ScriptableGameObjectManager()
	{
		if (m_ptrMonoDomain)
		{
			mono_jit_cleanup(m_ptrMonoDomain);
		}
	}
	void ScriptableGameObjectManager::UnloadDLL()
	{
		if (m_ptrMonoDomain)
		{
			mono_jit_cleanup(m_ptrMonoDomain);
		}
	}
	//----------------------------------------------------------------------------



	ECS::ScriptableEntity* ScriptableGameObjectManager::CreateEmptyGameObject()
	{
		ECS::ScriptableEntity* e = nullptr;
		e = &manager.addEntity();
		e->addComponent<ECS::ScriptComponent>().ClassName = "Transform";
		e->getComponent<ECS::ScriptComponent>().monoInstance = ScriptableGameObjectManager::GetInstance().CreateCsComponent("Transform");
		idGenerated++;
		return e;
	}

	//Operaciones desde menu TOP
	void ScriptableGameObjectManager::createGameObject(MainMenuAction action)
	{
		if (action == MainMenuAction::LoadComponentsFromCs)
		{
			ScriptableGameObjectManager::GetInstance().LoadDll();
		}
		else if (action == MainMenuAction::LiberateDLL)
		{
			ScriptableGameObjectManager::GetInstance().UnloadDLL();
		}
		else if (action == MainMenuAction::AddEmpty)
		{
			CreateEmptyGameObject();
		}
	}

	void ScriptableGameObjectManager::loadFileModel(GLCore::ImportOptions importOptions)
	{
	}

	void ScriptableGameObjectManager::drawHierarchy()
	{
	}











	//C# REFLECTIONS
	MonoObject* ScriptableGameObjectManager::CreateCsComponent(const std::string& className)
	{
		MonoClass* monoClass = mono_class_from_name(m_ptrCsEngineAssemblyImage, "CsEngineScript.ECS", className.c_str());
		if (monoClass)
		{
			MonoObject* instance = mono_object_new(m_ptrMonoDomain, monoClass);
			if (instance)
			{
				mono_runtime_object_init(instance);
				m_Classes[className] = monoClass;
				m_Instances[className] = instance;
				return instance;
			}
		}

		return nullptr;
	}
	void ScriptableGameObjectManager::InvokeMethod(const std::string& className, const std::string& methodName, void** params, int paramCount)
	{
		if (m_Instances.find(className) != m_Instances.end())
		{
			MonoMethod* method = mono_class_get_method_from_name(m_Classes[className], methodName.c_str(), paramCount);
			if (method)
			{
				mono_runtime_invoke(method, m_Instances[className], params, nullptr);
			}
		}
	}
	MonoObject* ScriptableGameObjectManager::GetCsComponentProperty(const std::string& className, const std::string& propertyName)
	{
		if (m_Instances.find(className) != m_Instances.end())
		{
			MonoMethod* getPropertyMethod = mono_class_get_method_from_name(m_Classes[className], "GetSerializableProperty", 1);
			if (getPropertyMethod)
			{
				void* args[1];
				MonoString* monoPropertyName = mono_string_new(m_ptrMonoDomain, propertyName.c_str());
				args[0] = monoPropertyName;

				return mono_runtime_invoke(getPropertyMethod, m_Instances[className], args, nullptr);
			}
		}
		return nullptr;
	}
	void ScriptableGameObjectManager::SetCsComponentProperty(const std::string& className, const std::string& propertyName, MonoObject* value)
	{
		if (m_Instances.find(className) != m_Instances.end())
		{
			MonoMethod* setPropertyMethod = mono_class_get_method_from_name(m_Classes[className], "SetSerializableProperty", 2);
			if (setPropertyMethod)
			{
				void* args[2];
				MonoString* monoPropertyName = mono_string_new(m_ptrMonoDomain, propertyName.c_str());
				args[0] = monoPropertyName;
				args[1] = value;

				mono_runtime_invoke(setPropertyMethod, m_Instances[className], args, nullptr);
			}
		}
	}

}