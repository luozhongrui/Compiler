#include "sema/SemanticError.hpp"
#include <memory>
// extern char *code[210];
// extern uint32_t if_error;
// decl

template <typename... Args>
static std::string str_format(const std::string &format, Args... args) {
  auto size_buf = std::snprintf(nullptr, 0, format.c_str(), args...) + 1;
  std::unique_ptr<char[]> buf(new (std::nothrow) char[size_buf]);

  if (!buf)
    return std::string("");

  std::snprintf(buf.get(), size_buf, format.c_str(), args...);
  return std::string(buf.get(), buf.get() + size_buf - 1);
}
std::string SemanticError::generateNotation(int offset) {
  std::string notation = "";
  for (int i = 1; i < offset; i++) {
    notation += " ";
  }
  notation += "^";
  return notation;
}

std::vector<std::string> SemanticError::getErrorMessage() { return messages; }

void SemanticError::errorSymTable(int line, int col, std::string symbolName) {
  if_error = 1;
  std::string source(code[line]);
  std::string notation = generateNotation(col);
  std::string message = str_format("<Error> Found in line %d, column %d: "
                                   "symbol '%s' is redeclared\n    %s\n    %s",
                                   line, col, symbolName.c_str(),
                                   source.c_str(), notation.c_str());

  messages.push_back(message);
}

// var decl
void SemanticError::errorVarDecl(VariableNode &p_variable) {
  if_error = 1;
  int line = p_variable.getLocation().line;
  int col = p_variable.getLocation().col;
  std::string source(code[line]);
  std::string notation = generateNotation(col);
  std::string symbolName = p_variable.getNameCString();
  std::string message = str_format(
      "<Error> Found in line %d, column %d: '%s' declared as an array with an "
      "index that is not greater than 0\n    %s\n    %s",
      line, col, symbolName.c_str(), source.c_str(), notation.c_str());
  messages.push_back(message);
}
// var refernece
void SemanticError::errorVarSymTable(VariableReferenceNode &p_variable_ref) {
  if_error = 1;
  int line = p_variable_ref.getLocation().line;
  int col = p_variable_ref.getLocation().col;
  std::string source(code[line]);
  std::string notation = generateNotation(col);
  std::string symbolName = p_variable_ref.getNameCString();
  std::string message = str_format("<Error> Found in line %d, column %d: use "
                                   "of undeclared symbol '%s'\n    %s\n    %s",
                                   line, col, symbolName.c_str(),
                                   source.c_str(), notation.c_str());
  messages.push_back(message);
}
void SemanticError::errorVarParameter(VariableReferenceNode &p_variable_ref) {
  if_error = 1;
  int line = p_variable_ref.getLocation().line;
  int col = p_variable_ref.getLocation().col;
  std::string source(code[line]);
  std::string notation = generateNotation(col);
  std::string symbolName = p_variable_ref.getNameCString();
  std::string message = str_format(
      "<Error> Found in line %d, column %d: use of non-variable symbol '%s'\n  "
      "  %s\n    %s",
      line, col, symbolName.c_str(), source.c_str(), notation.c_str());
  messages.push_back(message);
}
void SemanticError::errorVarIndexType(int line, int col) {
  if_error = 1;
  std::string source(code[line]);
  std::string notation = generateNotation(col);
  std::string message =
      str_format("<Error> Found in line %d, column %d: index of array "
                 "reference must be an integer\n    %s\n    %s",
                 line, col, source.c_str(), notation.c_str());
  messages.push_back(message);
}
void SemanticError::errorVarSubscript(VariableReferenceNode &p_variable_ref) {
  if_error = 1;
  int line = p_variable_ref.getLocation().line;
  int col = p_variable_ref.getLocation().col;
  std::string source(code[line]);
  std::string notation = generateNotation(col);
  std::string symbolName = p_variable_ref.getNameCString();
  std::string message = str_format(
      "<Error> Found in line %d, column %d: there is an over array subscript "
      "on '%s'\n    %s\n    %s",
      line, col, symbolName.c_str(), source.c_str(), notation.c_str());
  messages.push_back(message);
}

// binary unary operator
void SemanticError::errorBinOp(int line, int col, std::string op,
                               std::string left, std::string right) {
  if_error = 1;
  std::string source(code[line]);
  std::string notation = generateNotation(col);
  std::string message =
      str_format("<Error> Found in line %d, column %d: invalid operands to "
                 "binary operator '%s' ('%s' and '%s')\n    %s\n    %s",
                 line, col, op.c_str(), left.c_str(), right.c_str(),
                 source.c_str(), notation.c_str());
  messages.push_back(message);
}
void SemanticError::errorUnOp(int line, int col, std::string op,
                              std::string operant) {
  if_error = 1;
  std::string source(code[line]);
  std::string notation = generateNotation(col);
  std::string message = str_format(
      "<Error> Found in line %d, column %d: invalid operand to unary operator "
      "'%s' ('%s')\n    %s\n    %s",
      line, col, op.c_str(), operant.c_str(), source.c_str(), notation.c_str());
  messages.push_back(message);
}

// function invocation
void SemanticError::errorFuncSymTable(
    FunctionInvocationNode &p_func_invocation) {
  if_error = 1;
  int line = p_func_invocation.getLocation().line;
  int col = p_func_invocation.getLocation().col;
  std::string source(code[line]);
  std::string notation = generateNotation(col);
  std::string message =
      str_format("<Error> Found in line %d, column %d: use of undeclared "
                 "symbol '%s'\n    %s\n    %s",
                 line, col, p_func_invocation.getNameCString(), source.c_str(),
                 notation.c_str());
  messages.push_back(message);
}
void SemanticError::errorFuncSymNotFunc(
    FunctionInvocationNode &p_func_invocation) {
  if_error = 1;
  int line = p_func_invocation.getLocation().line;
  int col = p_func_invocation.getLocation().col;
  std::string source(code[line]);
  std::string notation = generateNotation(col);
  std::string message =
      str_format("<Error> Found in line %d, column %d: call of non-function "
                 "symbol '%s'\n    %s\n    %s",
                 line, col, p_func_invocation.getNameCString(), source.c_str(),
                 notation.c_str());
  messages.push_back(message);
}
void SemanticError::errorFuncParameter(
    FunctionInvocationNode &p_func_invocation) {
  if_error = 1;
  int line = p_func_invocation.getLocation().line;
  int col = p_func_invocation.getLocation().col;
  std::string source(code[line]);
  std::string notation = generateNotation(col);
  std::string message =
      str_format("<Error> Found in line %d, column %d: too few/much arguments "
                 "provided for function '%s'\n    %s\n    %s",
                 line, col, p_func_invocation.getNameCString(), source.c_str(),
                 notation.c_str());
  messages.push_back(message);
}
void SemanticError::errorFuncExpr(int line, int col, std::string passing,
                                  std::string paramter) {
  if_error = 1;
  std::string source(code[line]);
  std::string notation = generateNotation(col);
  std::string message =
      str_format("<Error> Found in line %d, column %d: incompatible type "
                 "passing '%s' to parameter of type '%s'\n    %s\n    %s",
                 line, col, passing.c_str(), paramter.c_str(), source.c_str(),
                 notation.c_str());
  messages.push_back(message);
}

// Print read
void SemanticError::errorPrintType(int line, int col) {
  if_error = 1;
  std::string source(code[line]);
  std::string notation = generateNotation(col);
  std::string message =
      str_format("<Error> Found in line %d, column %d: expression of print "
                 "statement must be scalar type\n    %s\n    %s",
                 line, col, source.c_str(), notation.c_str());
  messages.push_back(message);
}
void SemanticError::errorReadType(int line, int col) {
  if_error = 1;
  std::string source(code[line]);
  std::string notation = generateNotation(col);
  std::string message =
      str_format("<Error> Found in line %d, column %d: variable reference of "
                 "read statement must be scalar type\n    %s\n    %s",
                 line, col, source.c_str(), notation.c_str());
  messages.push_back(message);
}
void SemanticError::errorReadVar(int line, int col) {
  if_error = 1;
  std::string source(code[line]);
  std::string notation = generateNotation(col);
  std::string message =
      str_format("<Error> Found in line %d, column %d: variable reference of "
                 "read statement "
                 "cannot be a constant or loop variable\n    %s\n    %s",
                 line, col, source.c_str(), notation.c_str());
  messages.push_back(message);
}

// assignment
void SemanticError::errorAssignArrayType(int line, int col) {
  if_error = 1;
  std::string source(code[line]);
  std::string notation = generateNotation(col);
  std::string message = str_format("<Error> Found in line %d, column %d: array "
                                   "assignment is not allowed\n    %s\n    %s",
                                   line, col, source.c_str(), notation.c_str());
  messages.push_back(message);
}
void SemanticError::errorAssignConstType(int line, int col, std::string var) {
  if_error = 1;
  std::string source(code[line]);
  std::string notation = generateNotation(col);
  std::string message = str_format(
      "<Error> Found in line %d, column %d: cannot assign to variable "
      "'%s' which is a constant\n    %s\n    %s",
      line, col, var.c_str(), source.c_str(), notation.c_str());
  messages.push_back(message);
}
void SemanticError::errorAssignVarRefLoop(int line, int col) {
  if_error = 1;
  std::string source(code[line]);
  std::string notation = generateNotation(col);
  std::string message =
      str_format("<Error> Found in line %d, column %d: the value of loop "
                 "variable cannot be "
                 "modified inside the loop body\n    %s\n    %s",
                 line, col, source.c_str(), notation.c_str());
  messages.push_back(message);
}
void SemanticError::errorAssignResult(int line, int col) {
  if_error = 1;
  std::string source(code[line]);
  std::string notation = generateNotation(col);
  std::string message = str_format("<Error> Found in line %d, column %d: array "
                                   "assignment is not allowed\n    %s\n    %s",
                                   line, col, source.c_str(), notation.c_str());
  messages.push_back(message);
}
void SemanticError::errorAssignLvalue(int line, int col, std::string var,
                                      std::string expr) {
  if_error = 1;
  std::string source(code[line]);
  std::string notation = generateNotation(col);
  std::string message = str_format(
      "<Error> Found in line %d, column %d: assigning to "
      "'%s' from incompatible type "
      "'%s'\n    %s\n    %s",
      line, col, var.c_str(), expr.c_str(), source.c_str(), notation.c_str());
  messages.push_back(message);
}

// if and while
void SemanticError::errorIFWhileResult(int line, int col) {
  if_error = 1;
  std::string source(code[line]);
  std::string notation = generateNotation(col);
  std::string message =
      str_format("<Error> Found in line %d, column %d: the expression of "
                 "condition must be boolean type\n    %s\n    %s",
                 line, col, source.c_str(), notation.c_str());
  messages.push_back(message);
}

// for
void SemanticError::errorForInitial(int line, int col) {
  if_error = 1;
  std::string source(code[line]);
  std::string notation = generateNotation(col);
  std::string message = str_format(
      "<Error> Found in line %d, column %d: the lower bound and upper bound of "
      "iteration count must be in the incremental order\n    %s\n    %s",
      line, col, source.c_str(), notation.c_str());
  messages.push_back(message);
}

// Return
void SemanticError::errorReturnVoid(int line, int col) {
  if_error = 1;
  std::string source(code[line]);
  std::string notation = generateNotation(col);
  std::string message =
      str_format("<Error> Found in line %d, column %d: program/procedure "
                 "should not return a value\n    %s\n    %s",
                 line, col, source.c_str(), notation.c_str());
  messages.push_back(message);
}
void SemanticError::errorReturnType(int line, int col, std::string expr,
                                    std::string ret) {
  if_error = 1;
  std::string source(code[line]);
  std::string notation = generateNotation(col);
  std::string message = str_format(
      "<Error> Found in line %d, column %d: return "
      "'%s' from a "
      "function with return type '%s'\n    %s\n    %s",
      line, col, expr.c_str(), ret.c_str(), source.c_str(), notation.c_str());
  messages.push_back(message);
}