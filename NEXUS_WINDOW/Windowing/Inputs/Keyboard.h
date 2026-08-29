#pragma once
#include "Keys.h"
#include "Button.h"
#include <map>

namespace NEXUS_WINDOWING::Inputs {
	class Keyboard
	{
	private:
		std::map<int, Button> m_mapButtons;

	public:
		Keyboard();
		~Keyboard() = default;

		void Update();
		void OnKeyPressed(int key);
		void OnKeyReleased(int key);

		bool IsKeyPressed(int key) const;
		bool IsKeyJustPressed(int key) const;
		bool IsKeyJustReleased(int key) const;
	};
}
