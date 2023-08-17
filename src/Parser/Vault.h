#pragma once
#include "Core/Ref.h"
#include "Logger/LOG.h"
#include "Types/VarObject.h"
#include <map>

namespace Incantation
{
	struct FunctionExpression;
	struct IdentifierExpression;
	//struct VarObject {};

	/// <summary>
	/// This class is used by the back-end of the Incantation compiler. So in this repo it is left just empty.
	/// Class in the back-end is used to resolve varible tables and function overloadings
	/// </summary>
	class Vault
	{
	public:
		Vault();
		Vault(const Ptr<Vault>& previous);
		Vault(Vault* previous);

		void AddVar(const Ptr<IdentifierExpression>& var);

		bool IsVar(const Ptr<IdentifierExpression>& var);

		Ptr<VarObject>& GetVar(const Ptr<IdentifierExpression>& name);

		void AddFunction(const Ptr<FunctionExpression>& function);
		bool IsLocalFunc(const std::string& name, size_t argSize);
		Ptr<FunctionExpression> GetFunctionLocal(const std::string& name, size_t argSize);

		inline void SetPrevious(Ptr<Vault> vault) { this->prev = vault; }
		const Ptr<Vault>& GetPrevious();

		void Verbose();
	private:
		bool IsFunc(const std::string& name, size_t argSize);

		Ptr<Vault> prev = nullptr;
		std::unordered_map<std::string, Ptr<VarObject>> mVars;
		std::multimap<std::string, Ptr<FunctionExpression>> mFuncs{};
	};
}