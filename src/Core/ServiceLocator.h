#pragma once

namespace Incantation
{
	/// <summary>
	/// Based on this repository:
	/// https://github.com/Seng3694/ServiceLocator
	/// </summary>
	class ServiceLocator
	{
	public:
		ServiceLocator() : mSingletons() {};
		~ServiceLocator() { clear(); }

		void clear()
		{
			mSingletons.clear();
		}

		template<typename T, class ...Args>
		std::shared_ptr<T> registerInstance(Args... args)
		{
			const size_t hash = typeid(T).hash_code();
			T* instance = new T(args...);
			mSingletons.emplace(hash, std::shared_ptr<void>(instance));
			return resolve<T>();
		}

		template<typename T>
		std::shared_ptr<T> resolve() const
		{
			const size_t hash = typeid(T).hash_code();
			auto itr1 = mSingletons.find(hash);
			if (itr1 != mSingletons.end())
				return std::static_pointer_cast<T>(itr1->second);

			return nullptr;
		}

	private:
		std::unordered_map<size_t, std::shared_ptr<void>> mSingletons;
	};
}