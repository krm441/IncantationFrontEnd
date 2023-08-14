#pragma once
#include "Logger/LOG.h"
#include "Core/SafeCall.h"
#include "Parser/ASTnodes.h"

namespace Incantation
{
	inline int indentation = 0;
	inline void Tab() { indentation += 2; }
	inline void AltTab() { indentation -= 2; }
	inline std::string GetTab() { return std::string(indentation, ' '); }

	struct Visitor
	{
		mutable Console console;

		template<typename T>
		void operator()(const T& t) const
		{
			console.log("unknown type");
			throw StopCallInc("STOP::unknown type");
		}

		void operator()(const Ptr<Program>& program) const
		{
			this->console.log(GetTab(), "Body");
			console.log(GetTab(), '{');
			Tab();
			console.log(GetTab(), "Children size::", program->block->children.size());
			for (auto& child : program->block->children)
			{
				std::visit(Visitor{}, child);
			}
			AltTab();
			console.log(GetTab(), '}');
		}

		void operator()(const Ptr<FunctionExpression>& function) const
		{
			console.log(GetTab(), "Function:");
			console.log(GetTab(), '{');
			Tab();
			std::visit(Visitor(), AstNode(function->id));
			std::visit(Visitor(), AstNode(function->params));
			std::visit(Visitor(), AstNode(function->block));
			AltTab();
			console.log(GetTab(), '}');
		}

		void operator()(const Ptr<IdentifierExpression>& identifier)
		{
			console.log(std::string(indentation, ' '), "Identifier:", identifier->id.Value());
		}

		void operator()(const Ptr<String>& str)
		{
			console.log(GetTab(), "Identifier-string:", str->value);
		}

		void operator()(const Ptr<FunctionCall>& func)
		{
			console.log(GetTab(), "Function call: Name:", func->id.Value());
			console.log(GetTab(), '{');
			Tab();
			if (func->args.empty())
				console.log(GetTab(), "Args::No args");
			else
				for (auto& a : func->args)
				{
					console.log(GetTab(), "Arg::");
					Tab();
					std::visit(Visitor(), a);
					AltTab();
				}
			AltTab();
			console.log(GetTab(), '}');
		}

		void operator()(const Ptr<ParamsList>& list)
		{
			if (list->params.empty())
			{
				console.log(GetTab(), "Param list: empty");
				return;
			}
			console.log(GetTab(), "Param list:");
			for (auto& a : list->params)
				std::visit(Visitor(), AstNode(a));
		}

		void operator()(const Ptr<BlockStatement>& block)
		{
			console.log(GetTab(), "Block: Number of Children:", block->children.size());
			console.log(GetTab(), '{');
			Tab();
			for (auto& elem : block->children)
				std::visit(Visitor(), AstNode(elem));
			AltTab();
			console.log(GetTab(), '}');
		}

		void operator()(const Ptr<VarDeclaration>& var)
		{
			console.log(GetTab(), "Varible declaration:");
			console.log(GetTab(), '{');
			Tab();
			std::visit(Visitor(), AstNode(var->name));
			if (var->expression != nullptr)
			{
				std::visit(Visitor(), AstNode(var->expression));
			}
			AltTab();
			console.log(GetTab(), '}');
		}

		void operator()(const Ptr<AssignmentExpression>& ass)
		{
			console.log(GetTab(), "Assignment operation");
			console.log(GetTab(), '{');
			Tab();
			std::visit(Visitor(), AstNode(ass->left));
			console.log(GetTab(), "Operator:", ass->Op);
			std::visit(Visitor(), AstNode(ass->right));
			AltTab();
			console.log(GetTab(), '}');
		}

		void operator()(const Ptr<Array>& arr)
		{
			console.log(GetTab(), "Array expression. Items:", arr->items.size());
			console.log(GetTab(), '{');
			Tab();
			for (auto& a : arr->items)
				std::visit(Visitor(), a);
			AltTab();
			console.log(GetTab(), '}');
		}

		void operator()(const Ptr<BinaryExpression>& bin)
		{
			console.log(GetTab(), "Binary operation");
			console.log(GetTab(), '{');
			Tab();
			std::visit(Visitor(), AstNode(bin->left));
			console.log(GetTab(), "Operator:", bin->op);
			std::visit(Visitor(), AstNode(bin->right));
			AltTab();
			console.log(GetTab(), '}');
		}

		void operator()(const Ptr<UnaryExpression>& ptr)
		{
			console.log(GetTab(), "Unary expression");
			Tab();
			console.log(GetTab(), "Type:", ptr->type);
			console.log(GetTab(), "Argument:");
			Tab();
			std::visit(Visitor(), ptr->argument);
			AltTab();
			AltTab();
		}

		void operator()(const Ptr<UpdateExpression>& ptr)
		{
			console.log(GetTab(), "Update Expression:");
			Tab();
			console.log(GetTab(), "Type:", ptr->type);
			console.log(GetTab(), "Argument:");
			Visitor vis;
			vis(ptr->argument);
			AltTab();
		}

		void operator()(const Ptr<Literal>& literal)
		{
			console.log(GetTab(), "Literal::", literal->value);
		}

		void operator()(const Ptr<ReturnStatement>& ret)
		{
			console.log(GetTab(), "Return statement:");
			
			if (std::holds_alternative<std::shared_ptr<NullPtr>>(ret->argument))
			{
				console.log(GetTab(), '{');
				Tab();
				console.log(GetTab(), "Arguments: null");
				AltTab();
				console.log(GetTab(), '}');
			}
			else
			{
				console.log(GetTab(), '{');
				Tab();
				console.log(GetTab(), "Arguments::");
				std::visit(Visitor(), ret->argument);
				AltTab();
				console.log(GetTab(), '}');
			}
		}

		void operator() (const Ptr<BreakStatement>& brek)
		{
			console.log(GetTab(), "Break Statement");
		}

		void operator()(const Ptr<TestStatement>& test)
		{
			std::visit(Visitor(), test->test);
		}

		void operator()(const Ptr<IfStatement>& if_st)
		{
			console.log(GetTab(), "If statement:");
			Tab();
			console.log(GetTab(), "Test:");
			Tab();
			std::visit(Visitor(), AstNode(if_st->test->test));
			AltTab();
			console.log(GetTab(), "Consequent:");
			Tab();
			std::visit(Visitor(), AstNode(if_st->consequent));
			AltTab();
			console.log(GetTab(), "Alternate:");
			Tab();
			std::visit(Visitor(), if_st->alternate);
			AltTab();
			AltTab();
		}

		void operator()(const Ptr<WhileStatement>& whl)
		{
			console.log(GetTab(), "While statement:");
			console.log(GetTab(), '{');
			Tab();
			console.log(GetTab(), "Test:");
			Tab();
			std::visit(Visitor(), AstNode(whl->test->test));
			AltTab();
			console.log(GetTab(), "Consequent:");
			Tab();
			std::visit(Visitor(), AstNode(whl->body));
			AltTab();
			AltTab();
			console.log(GetTab(), '}');
		}

		void operator()(const Ptr<LogicalExpression>& ptr)
		{
			console.log(GetTab(), "Logical Expression:");
			console.log(GetTab(), '{');
			Tab();
			std::visit(Visitor(), ptr->left);
			console.log(GetTab(), "Operator:", LogicToStr(ptr->op));
			std::visit(Visitor(), ptr->right);
			AltTab();
			console.log(GetTab(), '}');
		}

		void operator()(const Ptr<NullPtr>& null)
		{
			console.log(GetTab(), "None.");
		}
	};

	inline void VerboseAST(const AstNode& node)
	{
		std::visit(Visitor{}, node);
	}
}