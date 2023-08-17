#include "Core/pchInc.h"
#include "Vault.h"
#include "ASTnodes.h"

using namespace Incantation;

Incantation::Vault::Vault()
{
}

Incantation::Vault::Vault(const Ptr<Vault>& previous) : prev(previous)
{
}

Incantation::Vault::Vault(Vault* previous) : prev(previous)
{
}

void Incantation::Vault::AddVar(const Ptr<IdentifierExpression>& var)
{
	mVars.insert(std::make_pair(var->id.Value(), create<VarObject>()));
}

bool Incantation::Vault::IsVar(const Ptr<IdentifierExpression>& var)
{
	if (mVars.find(var->id.Value()) != mVars.end())
		return true;
	if (prev != nullptr)
		return prev->IsVar(var);
	else return false;
}

Ptr<VarObject>& Incantation::Vault::GetVar(const Ptr<IdentifierExpression>& name)
{
	if (mVars.find(name->id.Value()) != mVars.end())
	{
		return mVars[name->id.Value()];
	}
	if (prev != nullptr)
		return prev->GetVar(name);
	throw StopCallInc("Not found VAR in VAULT");
}

void Incantation::Vault::AddFunction(const Ptr<FunctionExpression>& function)
{
	if(!IsFunc(function->id->id.Value(), function->params->params.size()))
		mFuncs.insert(std::make_pair(function->id->id.Value(), function));
}

bool Incantation::Vault::IsLocalFunc(const std::string& name, size_t argSize)
{
	if (mFuncs.find(name) != mFuncs.end())
	{
		// Function is already there -> lets see if it is an overload
		auto range = mFuncs.equal_range(name);

		for (auto it = range.first; it != range.second; ++it)
		{
			auto argsSize = it->second->params->params.size();
			if (argsSize == argSize)
			{
				return true;
			}
		}
	}

	if (prev)
	{
		return prev->IsLocalFunc(name, argSize);
	}

	return false;
}

Ptr<FunctionExpression> Incantation::Vault::GetFunctionLocal(const std::string& name, size_t argSize)
{
	if (mFuncs.find(name) != mFuncs.end())
	{
		// Function is already there -> lets see if it is an overload
		auto range = mFuncs.equal_range(name);

		for (auto it = range.first; it != range.second; ++it)
		{
			auto argsSize = it->second->params->params.size();
			if (argsSize == argSize)
			{
				return it->second;
			}
		}
	}

	if (prev)
	{
		return prev->GetFunctionLocal(name, argSize);
	}

	// It shouldn't get here
	throw SyntaxError("internal error : Vault::GetFunctionLocal " + name);
}

const Ptr<Vault>& Incantation::Vault::GetPrevious() { assert(prev); return prev; }

void Incantation::Vault::Verbose()
{
	Console console;
	if (mVars.empty() && mFuncs.empty())
	{
		console.log("\nEmpty vault"); 
		console.log("Previous", (prev ? "exists" : "none"));
		console.nl();
		return;
	}
	console.log("\nVerbosing Vault: vars:", mVars.size());
	for (auto& a : mVars)
	{
		console.log("Var:", a.first);
	}
	console.log("funcs", mFuncs.size());
	for (auto& a : mFuncs)
	{
		console.log("Function:", a.first);
	}
	console.log("Previous", prev);
	console.nl();
}

bool Incantation::Vault::IsFunc(const std::string& name, size_t argSize)
{
	if (mFuncs.find(name) != mFuncs.end())
	{
		// Function is already there -> lets see if it is an overload
		auto range = mFuncs.equal_range(name);

		for (auto it = range.first; it != range.second; ++it)
		{
			auto argsSize = it->second->params->params.size();
			if (argsSize == argSize)
			{
				Verbose();
				throw SyntaxError("Function oveload fail: \'" + name + "\' ::Same name and same ammount of args - error");
			}
		}
	}

	if (prev)
	{
		return prev->IsFunc(name, argSize);
	}

	return false;
}
