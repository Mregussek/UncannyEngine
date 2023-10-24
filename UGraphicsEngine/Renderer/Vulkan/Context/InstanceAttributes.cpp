
#include "InstanceAttributes.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"
#include "UTools/Logger/Log.h"


namespace uncanny::vulkan
{


void FInstanceAttributes::Initialize()
{
	GatherAvailableVersion();
	GatherAvailableLayers();
	GatherAvailableExtensions();
}


b8 FInstanceAttributes::AddLayerName(const char* layerName)
{
	if (IsLayerAvailable(layerName))
	{
		m_RequestedLayers.push_back(layerName);
		return UTRUE;
	}

	return UFALSE;
}


b8 FInstanceAttributes::AddExtensionName(const char* extensionName)
{
	if (IsExtensionAvailable(extensionName))
	{
		m_RequestedExtensions.push_back(extensionName);
		return UTRUE;
	}

	return UFALSE;
}


b8 FInstanceAttributes::IsVersionAvailable(u32 apiVersion) const
{
	// Supported version has always higher value than default VK_API_VERSION_1_3 for example
	if (m_SupportedVersion >= apiVersion)
	{
		return UTRUE;
	}

	UWARN("Given {} Vulkan API version is not supported!", apiVersion);
	return UFALSE;
}


b8 FInstanceAttributes::IsExtensionRequested(const char* extensionName) const
{
	if (not extensionName)
	{
		UERROR("No extension name is given!");
		return UFALSE;
	}

	for (auto requestedExtensionName : m_RequestedExtensions)
	{
		if (std::strcmp(extensionName, requestedExtensionName) == 0)
		{
			return UTRUE;
		}
	}
	return UFALSE;
}


void FInstanceAttributes::GatherAvailableVersion()
{
	VkResult result{ vkEnumerateInstanceVersion(&m_SupportedVersion) };
	AssertVkAndThrow(result);

	u32 major = VK_API_VERSION_MAJOR(m_SupportedVersion);
	u32 minor = VK_API_VERSION_MINOR(m_SupportedVersion);
	m_FullApiVersion = VK_MAKE_API_VERSION(0, major, minor, 0);
}


void FInstanceAttributes::GatherAvailableLayers()
{
	u32 layerCount{ 0 };
	VkResult result = vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	AssertVkAndThrow(result);
	m_AvailableLayerProperties.resize(layerCount);
	result = vkEnumerateInstanceLayerProperties(&layerCount, m_AvailableLayerProperties.data());
	AssertVkAndThrow(result);
}


void FInstanceAttributes::GatherAvailableExtensions()
{
	u32 extensionCount{ 0 };
	VkResult result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	AssertVkAndThrow(result);
	m_AvailableExtensionsProperties.resize(extensionCount);
	result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, m_AvailableExtensionsProperties.data());
	AssertVkAndThrow(result);
}


b8 FInstanceAttributes::IsLayerAvailable(const char* layerName) const
{
	if (not layerName)
	{
		UERROR("No layer name is given!");
		return UFALSE;
	}

	for (auto& vkLayerProperties : m_AvailableLayerProperties)
	{
		if (std::strcmp(layerName, vkLayerProperties.layerName) == 0)
		{
			return UTRUE;
		}
	}
	return UFALSE;
}


b8 FInstanceAttributes::IsExtensionAvailable(const char* extensionName) const
{
	if (not extensionName)
	{
		UERROR("No extension name is given!");
		return UFALSE;
	}

	for (auto& vkExtensionProperties : m_AvailableExtensionsProperties)
	{
		if (std::strcmp(extensionName, vkExtensionProperties.extensionName) == 0)
		{
			return UTRUE;
		}
	}
	return UFALSE;
}


}
