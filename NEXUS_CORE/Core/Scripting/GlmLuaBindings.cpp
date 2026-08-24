#include "GlmLuaBindings.h"
#include <glm/glm.hpp>
#include <lua.hpp>
#include <LuaBridge3/LuaBridge.h>
#include <algorithm>
#include <cmath>

namespace NEXUS_CORE::Scripting {

	// glm::vec2
	void CreateVec2Bind(lua_State* L)
	{
		luabridge::getGlobalNamespace(L)
			.beginClass<glm::vec2>("vec2")
			.addConstructor<void (*)(float), void (*)(float, float)>()
			.addPropertyReadWrite("x", &glm::vec2::x)
			.addPropertyReadWrite("y", &glm::vec2::y)
			.addFunction("__mul",
				[](const glm::vec2& v1, const glm::vec2& v2) { return v1 * v2; },
				[](const glm::vec2& v1, float value) { return v1 * value; })
			.addFunction("__div",
				[](const glm::vec2& v1, const glm::vec2& v2) { return v1 / v2; },
				[](const glm::vec2& v1, float value) { return v1 / value; })
			.addFunction("__add",
				[](const glm::vec2& v1, const glm::vec2& v2) { return v1 + v2; },
				[](const glm::vec2& v1, float value) { return v1 + value; })
			.addFunction("__sub",
				[](const glm::vec2& v1, const glm::vec2& v2) { return v1 - v2; },
				[](const glm::vec2& v1, float value) { return v1 - value; })
			.endClass();
	}

	// glm::vec3
	void CreateVec3Bind(lua_State* L)
	{
		luabridge::getGlobalNamespace(L)
			.beginClass<glm::vec3>("vec3")
			.addConstructor<void (*)(float), void (*)(float, float, float)>()
			.addPropertyReadWrite("x", &glm::vec3::x)
			.addPropertyReadWrite("y", &glm::vec3::y)
			.addPropertyReadWrite("z", &glm::vec3::z)
			.addFunction("__mul",
				[](const glm::vec3& v1, const glm::vec3& v2) { return v1 * v2; },
				[](const glm::vec3& v1, float value) { return v1 * value; })
			.addFunction("__div",
				[](const glm::vec3& v1, const glm::vec3& v2) { return v1 / v2; },
				[](const glm::vec3& v1, float value) { return v1 / value; })
			.addFunction("__add",
				[](const glm::vec3& v1, const glm::vec3& v2) { return v1 + v2; },
				[](const glm::vec3& v1, float value) { return v1 + value; })
			.addFunction("__sub",
				[](const glm::vec3& v1, const glm::vec3& v2) { return v1 - v2; },
				[](const glm::vec3& v1, float value) { return v1 - value; })
			.endClass();
	}

	// glm::vec4
	void CreateVec4Bind(lua_State* L)
	{
		luabridge::getGlobalNamespace(L)
			.beginClass<glm::vec4>("vec4")
			.addConstructor<void (*)(float), void (*)(float, float, float, float)>()
			.addPropertyReadWrite("x", &glm::vec4::x)
			.addPropertyReadWrite("y", &glm::vec4::y)
			.addPropertyReadWrite("z", &glm::vec4::z)
			.addPropertyReadWrite("w", &glm::vec4::w)
			.addFunction("__mul",
				[](const glm::vec4& v1, const glm::vec4& v2) { return v1 * v2; },
				[](const glm::vec4& v1, float value) { return v1 * value; })
			.addFunction("__div",
				[](const glm::vec4& v1, const glm::vec4& v2) { return v1 / v2; },
				[](const glm::vec4& v1, float value) { return v1 / value; })
			.addFunction("__add",
				[](const glm::vec4& v1, const glm::vec4& v2) { return v1 + v2; },
				[](const glm::vec4& v1, float value) { return v1 + value; })
			.addFunction("__sub",
				[](const glm::vec4& v1, const glm::vec4& v2) { return v1 - v2; },
				[](const glm::vec4& v1, float value) { return v1 - value; })
			.endClass();
	}

	/*
	* Some helper math functions
	*/
	void MathFreeFunctions(lua_State* L)
	{
		luabridge::getGlobalNamespace(L)
			.addFunction("distance",
				[](const glm::vec2& a, const glm::vec2& b) { return glm::distance(a, b); },
				[](const glm::vec3& a, const glm::vec3& b) { return glm::distance(a, b); },
				[](const glm::vec4& a, const glm::vec4& b) { return glm::distance(a, b); })
			.addFunction("lerp", [](float a, float b, float t) { return std::lerp(a, b, t); })
			.addFunction("clamp", [](float value, float min, float max) { return std::clamp(value, min, max); });
	}

	void GLMBindings::CreateGLMBindings(lua_State* L)
	{
		CreateVec2Bind(L);
		CreateVec3Bind(L);
		CreateVec4Bind(L);

		MathFreeFunctions(L);
	}
}