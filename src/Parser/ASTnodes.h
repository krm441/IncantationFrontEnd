#pragma once
#include <variant>
#include <memory>
#include "Lexer/Token.h"
#include <vector>
#include "Core/Ref.h"
#include "Core/SafeCall.h"
#include <string>
#include <map>
#include <unordered_set>
#include <typeindex>
#include <unordered_map>
#include "Vault.h"

namespace Incantation
{
	enum class LogicalOperator
	{
		Null, AND, OR, NOT
	};

	inline std::string LogicToStr(const LogicalOperator& op)
	{
		switch (op)
		{
		case LogicalOperator::AND: return "AND"; break;
		case LogicalOperator::OR: return "OR"; break;
		case LogicalOperator::NOT: return "NOT"; break;
		default:
			throw SyntaxError("Not an operator");
			break;
		}
	}

	enum class OperatorType
	{
		Null, ASS, ADD, SUB, MUL, DIV, MOD,
		//<, >, <=, >=, ==, !=
		Less, Greater, LessEqual, GreaterEqual, Equal, NotEqual
	};

	/// <summary>
	/// Represents a terminal in AST - nullptr
	/// </summary>
	struct NullPtr {};
	struct Program;
	struct IdentifierExpression;
	struct ParamsList;
	struct BlockStatement;
	struct FunctionExpression;
	struct ReturnStatement;
	struct VarDeclaration;
	struct BinaryExpression;
	struct AssignmentExpression;
	struct BinaryExpression;
	struct UnaryExpression;
	struct UpdateExpression;
	struct Literal;
	struct IfStatement;
	struct WhileStatement;
	struct ForStatement;
	struct LogicalExpression;
	struct TestStatement;
	struct FunctionCall;
	struct RegisterALU;
	struct String;
	struct Array;
	struct BreakStatement {};

	using AstNode = std::variant<Ptr<NullPtr>,
		Ptr<Program>,				Ptr<IdentifierExpression>,
		Ptr<ParamsList>,			Ptr<BlockStatement>, Ptr<FunctionExpression>,
		Ptr<VarDeclaration>,		Ptr<BinaryExpression>, Ptr<AssignmentExpression>,
		Ptr<ReturnStatement>,		Ptr<IfStatement>, Ptr<WhileStatement>, Ptr<ForStatement>, Ptr<UnaryExpression>,
		Ptr<UpdateExpression>,		Ptr<Literal>,
		Ptr<LogicalExpression>,		Ptr<FunctionCall>, Ptr<RegisterALU>, Ptr<String>, Ptr<Array>, Ptr<TestStatement>,
		Ptr<BreakStatement>
	>;

	// Helper function to print the type of the variant
	template<typename... Ts>
	void PrintVariantType(const std::variant<Ts...>& var)
	{
		static std::unordered_map<std::type_index, std::string> typeNames =
		{
			{std::type_index(typeid(Ptr<NullPtr>)),					"NullPtr"},
			{std::type_index(typeid(Ptr<Program>)),					"Program"},
			{std::type_index(typeid(Ptr<IdentifierExpression>)),	"IdentifierExpression"},
			{std::type_index(typeid(Ptr<ParamsList>)),				"ParamsList"},
			{std::type_index(typeid(Ptr<BlockStatement>)),			"BlockStatement"},
			{std::type_index(typeid(Ptr<FunctionExpression>)),		"FunctionExpression"},
			{std::type_index(typeid(Ptr<ReturnStatement>)),			"ReturnStatement"},
			{std::type_index(typeid(Ptr<VarDeclaration>)),			"VarDeclaration"},
			{std::type_index(typeid(Ptr<BinaryExpression>)),		"BinaryExpression"},
			{std::type_index(typeid(Ptr<AssignmentExpression>)),	"AssignmentExpression"},
			{std::type_index(typeid(Ptr<BinaryExpression>)),		"BinaryExpression"},
			{std::type_index(typeid(Ptr<UnaryExpression>)),			"UnaryExpression"},
			{std::type_index(typeid(Ptr<UpdateExpression>)),		"UpdateExpression"},
			{std::type_index(typeid(Ptr<Literal>)),					"Literal"},
			{std::type_index(typeid(Ptr<IfStatement>)),				"IfStatement"},
			{std::type_index(typeid(Ptr<WhileStatement>)),			"WhileStatement"},
			{std::type_index(typeid(Ptr<ForStatement>)),			"ForStatement"},
			{std::type_index(typeid(Ptr<LogicalExpression>)),		"LogicalExpression"},
			{std::type_index(typeid(Ptr<TestStatement>)),			"TestStatement"},
			{std::type_index(typeid(Ptr<FunctionCall>)),			"FunctionCall"},
			{std::type_index(typeid(Ptr<RegisterALU>)),				"RegisterALU"},
			{std::type_index(typeid(Ptr<String>)),					"String"},
			{std::type_index(typeid(Ptr<Array>)),					"Array"},
			{std::type_index(typeid(Ptr<BreakStatement>)),			"BreakStatement"},
		};

		std::visit([&](const auto& value)
			{
				auto it = typeNames.find(std::type_index(typeid(value)));
				if (it != typeNames.end()) {
					std::cout << "Type: " << it->second << ", Value: " << value << std::endl;
				}
				else
				{
					std::cout << "Unknown type" << std::endl;
				}
			}, var);
	}

	/// <summary>
	/// Settings for scope control and for the 'vault' pointer wrapper 
	/// </summary>
	struct CodeBlockSettings
	{
		int GlobalScope = 0, LoopScope = 0, FunctionScope = 0;
		Vault* previous = nullptr;
	};

	struct BlockStatement
	{
		Ptr<Vault> vault;
		std::vector<AstNode> children;
	};

	struct Program
	{
		Program() { block = create<BlockStatement>(); }
		Ptr<BlockStatement> block;
	};

	struct BinaryExpression
	{
		AstNode left;
		OperatorType op = OperatorType::Null;
		AstNode right;
	};

	struct TestStatement
	{
		AstNode test;
	};

	struct VarDeclaration
	{
		Ptr<IdentifierExpression> name;
		Ptr<AssignmentExpression> expression;
	};

	struct IdentifierExpression
	{
		Token id;
	};

	struct FunctionCall
	{
		Token id;
		std::vector<AstNode> args;
		Ptr<RegisterALU> reg;
	};

	struct LogicalExpression
	{
		LogicalOperator op = LogicalOperator::Null;
		AstNode left;
		AstNode right;
	};

	struct FunctionExpression
	{
		Ptr<IdentifierExpression> id;
		Ptr<ParamsList> params;
		Ptr<BlockStatement> block;
		std::vector<std::string> text;
	};

	struct IfStatement
	{
		Ptr<TestStatement> test;
		Ptr<BlockStatement> consequent;
		AstNode alternate;
	};

	struct WhileStatement
	{
		Ptr<TestStatement> test;
		Ptr<BlockStatement> body;
	};

	struct ForStatement
	{
		std::variant<Ptr<VarDeclaration>, Ptr<BinaryExpression>> init;
		AstNode test;
		std::variant<Ptr<BinaryExpression>, Ptr<UpdateExpression>> update;
		Ptr<BlockStatement> body;
	};

	struct Array
	{
		std::vector<AstNode> items;
	};

	struct ParamsList
	{
		std::vector<Ptr<IdentifierExpression>> params;
	};

	struct ReturnStatement
	{
		AstNode argument;
	};

	std::ostream& operator<<(std::ostream& os, const OperatorType& value);

	enum class UnaryExpressionType
	{
		Null, Negate, UpdatePrefixDecrement, UpdatePrefixIncrement, UpdateSuffixDecrement, UpdateSuffixIncrement, NOT
	};

	inline std::ostream& operator<<(std::ostream& os, const UnaryExpressionType& value)
	{
		switch (value)
		{
		case UnaryExpressionType::Negate: os << "Negation"; break;
		case UnaryExpressionType::UpdatePrefixDecrement: os << "Update prefix decrement"; break;
		case UnaryExpressionType::UpdatePrefixIncrement: os << "Update prefix increment"; break;
		case UnaryExpressionType::UpdateSuffixDecrement: os << "Update suffix decrement"; break;
		case UnaryExpressionType::UpdateSuffixIncrement: os << "Update suffix increment"; break;
		case UnaryExpressionType::NOT: os << "NOT operation"; break;
		default: throw "unknown unary";
			break;
		}
		return os;
	}

	struct UnaryExpression
	{
		UnaryExpressionType type = UnaryExpressionType::Null;
		AstNode argument;
		Ptr<RegisterALU> reg;
	};

	struct UpdateExpression
	{
		UnaryExpressionType type = UnaryExpressionType::Null;
		Ptr<IdentifierExpression> argument;
	};

	struct Literal
	{
		Literal(double constant) : value(constant) {}
		double value = 0;
	};

	struct String
	{
		String(const std::string& str) : value(str) {}
		std::string value;
	};

	/// <summary>
	/// For backend use only
	/// </summary>
	struct RegisterALU
	{
		size_t pointer;
	};

	struct AssignmentExpression
	{
		Ptr<IdentifierExpression> left;
		OperatorType Op = OperatorType::Null;
		AstNode right;
	};
}