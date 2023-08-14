#pragma once
#include <mutex>
#include <iostream>
#include "Core/SafeCall.h"

namespace Incantation
{
	class Console
	{
	public:
		Console(bool on = true) : m_on(on) {};
		~Console() {};

		void off() { m_on = false; }
		void on() { m_on = true; }

		/// <summary>
		/// Log a new Line in the console - a kind of separ
		/// </summary>
		void nl() { log('\n'); }

		void gaff() { log("Incantation::gaff"); }

		void begin() { log("LOG::begin"); }
		void end() { log("LOG::end"); }

		template<class... Args>
		inline void log(Args... args)
		{
			std::unique_lock<std::mutex> m(m_mutex); //smart lock
			if (m_on)
			{
				std::cout << "Incantation:: ";// << first << ' ';
				((std::cout << args << ' '), ...) << std::endl;
			}
		}

	private:
		bool m_on;
		std::mutex m_mutex;
	};

}
