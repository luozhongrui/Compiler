#include "sema/SemanticAnalyzer.hpp"
#include "visitor/AstNodeInclude.hpp"

auto symbolManager = *(new SymbolManager());
auto errorMessage = new SemanticError();
int scopLevel = -1; // scop level
int ifArray = 0;
int varRefError = 0;
std::string varRefSymbol;
std::vector<constantInfo> constant;
// binary operator
std::stack<operandInfo> operand;
int ifVisitConst = 0;
int checkParent = 0;
int error = 0;

void SemanticAnalyzer::dumpDemarcation(const char chr) {
  for (size_t i = 0; i < 110; ++i) {
    printf("%c", chr);
  }
  puts("");
}
void SemanticAnalyzer::dumpSymbol(SymbolTable &table) {
  dumpDemarcation('=');

  printf("%-33s%-11s%-11s%-17s%-11s\n", "Name", "Kind", "Level", "Type",
         "Attribute");
  dumpDemarcation('-');
  auto entires = table.getSymbolTableList();
  for (auto entire = entires.begin(); entire != entires.end(); entire++) {
    std::string levelType = "(" + entire->getLevel().second + ")";
    printf("%-33s", entire->getName().c_str());
    printf("%-11s", entire->getKind().c_str());
    printf("%d%-10s", entire->getLevel().first, levelType.c_str());
    printf("%-17s", entire->getType().c_str());
    printf("%-11s", entire->getAttribute().c_str());
    puts("");
  }
  dumpDemarcation('-');
}

void SemanticAnalyzer::visit(ProgramNode &p_program) {
  /*
   * TODO:
   *
   * 1. Push a new symbol table if this node forms a scope.
   * 2. Insert the symbol into current symbol table if this node is related to
   *    declaration (ProgramNode, VariableNode, FunctionNode).
   * 3. Travere child nodes of this node.
   * 4. Perform semantic analyses of this node.
   * 5. Pop the symbol table pushed at the 1st step.
   */

  auto symbolTable = new SymbolTable("program");
  symbolManager.pushScope(symbolTable);
  auto symbol = *(new SymbolEntry(p_program.getNameCString(), "program", "void",
                                  "", {scopLevel, "global"}));
  symbolTable->addSymbol(symbol);
  p_program.visitChildNodes(*this);

  TABLEDUMP
  symbolManager.popScope();
  if (if_error) {
    auto message = errorMessage->getErrorMessage();
    for (auto it = message.begin(); it != message.end(); it++) {
      printf("%s\n", (*it).c_str());
    }
  }
  delete (errorMessage);
  // delete (&symbolManager);
}

void SemanticAnalyzer::visit(DeclNode &p_decl) {
  p_decl.visitChildNodes(*this);
}

void SemanticAnalyzer::visit(VariableNode &p_variable) {
  /*
   * TODO:
   *
   * 1. Push a new symbol table if this node forms a scope.
   * 2. Insert the symbol into current symbol table if this node is related to
   *    declaration (ProgramNode, VariableNode, FunctionNode).
   * 3. Travere child nodes of this node.src/lib/sema/SemanticAnalyzer.cpp
   * 4. Perform semantic analyses of this node.
   * 5. Pop the symbol table pushed at the 1st step.
   */
  auto symbolTable = symbolManager.getSybolTable();
  auto constantPtr = p_variable.getConstPtr();
  std::string name = p_variable.getNameCString();
  std::string kind, attribute;
  std::pair<int, std::string> level =
      std::make_pair(scopLevel, (scopLevel > 0) ? "local" : "global");
  if (constantPtr) {
    kind = "constant";
    attribute = constantPtr->getConstantValueCString();
  } else {
    kind = "variable";
    attribute = "";
  }
  if (functionPath) {
    kind = "parameter";
  }
  if (forPath) {
    kind = "loop_var";
    forPath = 0;
  }
  auto symbol = *(new SymbolEntry(name, kind, p_variable.getTypeCString(),
                                  attribute, level));

  if (!symbolManager.existTableSymbol(name, level.first)) {
    symbolTable->addSymbol(symbol);
  } else {
    errorMessage->errorSymTable(p_variable.getLocation().line,
                                p_variable.getLocation().col, name);
  }
  // type checking
  std::vector<uint64_t> varTypeDim = p_variable.getDimensions();
  for (auto i = 0; i < int(varTypeDim.size()); i++) {
    if (varTypeDim[i] <= 0) {
      errorMessage->errorVarDecl(p_variable);
      varRefSymbol = name;
      return;
    }
  }
  p_variable.visitChildNodes(*this);
}

void SemanticAnalyzer::visit(ConstantValueNode &p_constant_value) {
  /*
   * TODO:
   *
   * 1. Push a new symbol table if this node forms a scope.
   * 2. Insert the symbol into current symbol table if this node is related to
   *    declaration (ProgramNode, VariableNode, FunctionNode).
   * 3. Travere child nodes of this node.
   * 4. Perform semantic analyses of this node.
   * 5. Pop the symbol table pushed at the 1st step.
   */

  p_constant_value.ptype.setPrimitiveType(*p_constant_value.getTypeSharedPtr());
}

void SemanticAnalyzer::visit(FunctionNode &p_function) {
  /*
   * TODO:
   *
   * 1. Push a new symbol table if this node forms a scope.
   * 2. Insert the symbol into current symbol table if this node is related to
   *    declaration (ProgramNode, VariableNode, FunctionNode).
   * 3. Travere child nodes of this node.
   * 4. Perform semantic analyses of this node.
   * 5. Pop the symbol table pushed at the 1st step.
   */
  auto p_symbolTable = symbolManager.getSybolTable();
  auto symbol =
      *(new SymbolEntry(p_function.getNameCString(), "function",
                        p_function.getPrototypeNmaeCString(),
                        p_function.getPrototypeParametersTypeCString(),
                        {scopLevel, (scopLevel > 0) ? "local" : "global"}));
  if (!symbolManager.existTableSymbol(p_function.getNameCString(), scopLevel)) {
    p_symbolTable->addSymbol(symbol);
  } else {
    errorMessage->errorSymTable(p_function.getLocation().line,
                                p_function.getLocation().col,
                                p_function.getNameCString());
  }
  auto symbolTable = new SymbolTable("function");
  symbolManager.pushScope(symbolTable);
  functionPath = 1;
  p_function.visitChildNodes(*this);
  TABLEDUMP
  symbolManager.popScope();
}

void SemanticAnalyzer::visit(CompoundStatementNode &p_compound_statement) {
  /*
   * TODO:
   *
   * 1. Push a new symbol table if this node forms a scope.
   * 2. Insert the symbol into current symbol table if this node is related to
   *    declaration (ProgramNode, VariableNode, FunctionNode).
   * 3. Travere child nodes of this node.
   * 4. Perform semantic analyses of this node.
   * 5. Pop the symbol table pushed at the 1st step.
   */
  if (functionPath) {
    functionPath = !functionPath;
    p_compound_statement.visitChildNodes(*this);

  } else {
    auto symbolTable = new SymbolTable("compound");
    symbolManager.pushScope(symbolTable);
    p_compound_statement.visitChildNodes(*this);
    TABLEDUMP
    symbolManager.popScope();
  }
}

void SemanticAnalyzer::visit(PrintNode &p_print) {
  /*
   * TODO:
   *
   * 1. Push a new symbol table if this node forms a scope.
   * 2. Insert the symbol into current symbol table if this node is related to
   *    declaration (ProgramNode, VariableNode, FunctionNode).
   * 3. Travere child nodes of this node.
   * 4. Perform semantic analyses of this node.
   * 5. Pop the symbol table pushed at the 1st step.
   */
  p_print.visitChildNodes(*this);
  auto childType = p_print.getChildren()->ptype;
  if (childType.isError()) {
    return;
  }
  if (!childType.isScalar()) {
    errorMessage->errorPrintType(p_print.getChildren()->getLocation().line,
                                 p_print.getChildren()->getLocation().col);
    return;
  }
}

void SemanticAnalyzer::visit(BinaryOperatorNode &p_bin_op) {
  /*
   * TODO:
   *
   * 1. Push a new symbol table if this node forms a scope.
   * 2. Insert the symbol into current symbol table if this node is related to
   *    declaration (ProgramNode, VariableNode, FunctionNode).
   * 3. Travere child nodes of this node.
   * 4. Perform semantic analyses of this node.
   * 5. Pop the symbol table pushed at the 1st step.
   */
  std::string op = p_bin_op.getOpCString();
  p_bin_op.visitChildNodes(*this);
  auto leftType = p_bin_op.getLeft()->ptype;
  auto rightType = p_bin_op.getRight()->ptype;
  if (leftType.isError() || rightType.isError()) {
    p_bin_op.ptype.setPrimitiveType(uint8_t(5));
    return;
  }

  if (leftType.isString() || rightType.isString()) {
    if (op.compare("+")) {
      errorMessage->errorBinOp(
          p_bin_op.getLocation().line, p_bin_op.getLocation().col, op,
          leftType.getPTypeCString(), rightType.getPTypeCString());
      p_bin_op.ptype.setPrimitiveType(uint8_t(5));
      return;
    } else {
      if (!leftType.compare(rightType)) {
        errorMessage->errorBinOp(
            p_bin_op.getLocation().line, p_bin_op.getLocation().col, op,
            leftType.getPTypeCString(), rightType.getPTypeCString());
        p_bin_op.ptype.setPrimitiveType(uint8_t(5));
        return;
      } else {
        p_bin_op.ptype.setPrimitiveType(leftType.wilderType(rightType));
      }
    }
    return;
  }

  // add "and" check only int, bool allowed, must reject real
  if (!op.compare("and") || !op.compare("or")) {
    if (leftType.isReal() || rightType.isReal()) {
      errorMessage->errorBinOp(
          p_bin_op.getLocation().line, p_bin_op.getLocation().col, op,
          leftType.getPTypeCString(), rightType.getPTypeCString());
      p_bin_op.ptype.setPrimitiveType(uint8_t(5));
      return;
    } else {
      if (!leftType.compare(rightType)) {
        errorMessage->errorBinOp(
            p_bin_op.getLocation().line, p_bin_op.getLocation().col, op,
            leftType.getPTypeCString(), rightType.getPTypeCString());
        p_bin_op.ptype.setPrimitiveType(uint8_t(5));
        return;
      } else {
        p_bin_op.ptype.setPrimitiveType(leftType.wilderType(rightType));
      }
    }
    return;
  }

  if (op.compare("mod")) {
    if (!leftType.compare(rightType)) {
      errorMessage->errorBinOp(
          p_bin_op.getLocation().line, p_bin_op.getLocation().col, op,
          leftType.getPTypeCString(), rightType.getPTypeCString());
      p_bin_op.ptype.setPrimitiveType(uint8_t(5));
      return;
    } else {
      if (!op.compare("<") || !op.compare("<=") || !op.compare("=") ||
          !op.compare(">=") || !op.compare(">") || !op.compare("<>")) {
        p_bin_op.ptype.setPrimitiveType(uint8_t(3));
      } else {
        p_bin_op.ptype.setPrimitiveType(leftType.wilderType(rightType));
      }
    }
  } else {
    if (!leftType.compareMod(rightType)) {
      errorMessage->errorBinOp(
          p_bin_op.getLocation().line, p_bin_op.getLocation().col, op,
          leftType.getPTypeCString(), rightType.getPTypeCString());
      p_bin_op.ptype.setPrimitiveType(uint8_t(5));
      return;
    } else {
      p_bin_op.ptype.setPrimitiveType(leftType.wilderType(rightType));
    }
  }
}

void SemanticAnalyzer::visit(UnaryOperatorNode &p_un_op) {
  /*
   * TODO:
   *
   * 1. Push a new symbol table if this node forms a scope.
   * 2. Insert the symbol into current symbol table if this node is related to
   *    declaration (ProgramNode, VariableNode, FunctionNode).
   * 3. Travere child nodes of this node.
   * 4. Perform semantic analyses of this node.
   * 5. Pop the symbol table pushed at the 1st step.
   */
  std::string op = p_un_op.getOpCString();
  p_un_op.visitChildNodes(*this);
  auto childType = p_un_op.getChild()->ptype;
  if (childType.isError()) {
    p_un_op.ptype.setPrimitiveType(uint8_t(5));
    return;
  }

  if (!op.compare("-")) {
    if (childType.isReal() || childType.isInt()) {
      // p_un_op.ptype.setPrimitiveType(
      // p_un_op.getChild()->ptype.getPrimitiveType());
    } else {
      errorMessage->errorUnOp(p_un_op.getLocation().line,
                              p_un_op.getLocation().col, op,
                              childType.getPTypeCString());
      p_un_op.ptype.setPrimitiveType(uint8_t(5));
      return;
    }
  }
  if (!op.compare("not")) {
    if (!childType.isBool()) {
      errorMessage->errorUnOp(p_un_op.getLocation().line,
                              p_un_op.getLocation().col, op,
                              childType.getPTypeCString());
      p_un_op.ptype.setPrimitiveType(uint8_t(5));
      return;
    }
  }
  p_un_op.ptype.setPrimitiveType(p_un_op.getChild()->ptype.getPrimitiveType());
}

void SemanticAnalyzer::visit(FunctionInvocationNode &p_func_invocation) {
  /*
   * TODO:
   *
   * 1. Push a new symbol table if this node forms a scope.
   * 2. Insert the symbol into current symbol table if this node is related to
   *    declaration (ProgramNode, VariableNode, FunctionNode).
   * 3. Travere child nodes of this node.
   * 4. Perform semantic analyses of this node.
   * 5. Pop the symbol table pushed at the 1st step.
   */
  // TODO: check type of argument if fit the symbol table prototype of the
  std::string name = p_func_invocation.getNameCString();
  if (!symbolManager.existTableSymbol(name)) {
    errorMessage->errorFuncSymTable(p_func_invocation);
    p_func_invocation.ptype.setPrimitiveType(uint8_t(5));
    return;
  }
  auto symbol = symbolManager.getSymbol(name);
  if (symbol.getKind().compare("function")) {
    errorMessage->errorFuncSymNotFunc(p_func_invocation);
    p_func_invocation.ptype.setPrimitiveType(uint8_t(5));
    return;
  }

  std::vector<std::string> attribute = symbol.getAttributeList();
  p_func_invocation.visitChildNodes(*this);
  if (p_func_invocation.getChildren().size() != attribute.size()) {
    errorMessage->errorFuncParameter(p_func_invocation);
    p_func_invocation.ptype.setPrimitiveType(uint8_t(5));
    return;
  }
  for (size_t i = 0; i < attribute.size(); i++) {
    std::string childType =
        p_func_invocation.getChildren()[i]->ptype.getPTypeCString();
    if (childType.compare(attribute[i])) {
      errorMessage->errorFuncExpr(
          p_func_invocation.getChildren()[i]->getLocation().line,
          p_func_invocation.getChildren()[i]->getLocation().col,
          p_func_invocation.getChildren()[i]->ptype.getPTypeCString(),
          attribute[i]);
      p_func_invocation.ptype.setPrimitiveType(uint8_t(5));
      return;
    }
  }

  p_func_invocation.ptype.setPrimitiveType(symbol.getTypeNum(),
                                           symbol.getDim());
}

void SemanticAnalyzer::visit(VariableReferenceNode &p_variable_ref) {
  /*
   * TODO:
   *
   * 1. Push a new symbol table if this node forms a scope.
   * 2. Insert the symbol into current symbol table if this node is related to
   *    declaration (ProgramNode, VariableNode, FunctionNode).
   * 3. Travere child nodes of this node.
   * 4. Perform semantic analyses of this node.
   * 5. Pop the symbol table pushed at the 1st step.
   */
  std::string symbolName = p_variable_ref.getNameCString();

  // check
  if (!symbolManager.existTableSymbol(symbolName)) {
    errorMessage->errorVarSymTable(p_variable_ref);
    p_variable_ref.ptype.setPrimitiveType(uint8_t(5));
    return;
  }
  if (!symbolManager.existSymbolKind(symbolName)) {
    errorMessage->errorVarParameter(p_variable_ref);
    p_variable_ref.ptype.setPrimitiveType(uint8_t(5));
    return;
  }

  p_variable_ref.visitChildNodes(*this);

  auto symbol = symbolManager.getSymbol(symbolName);
  auto dim = symbol.getDim();
  auto &constant = p_variable_ref.getChildren();
  for (size_t i = 0; i < constant.size(); i++) {
    if (constant[i]->ptype.isReal() && varRefSymbol.compare(symbolName) != 0) {
      errorMessage->errorVarIndexType(constant[i]->getLocation().line,
                                      constant[i]->getLocation().col);
      p_variable_ref.ptype.setPrimitiveType(uint8_t(5));
      return;
    }
  }
  if (p_variable_ref.getChildren().size() > dim.size() &&
      varRefSymbol.compare(symbolName) != 0) {
    errorMessage->errorVarSubscript(p_variable_ref);
    p_variable_ref.ptype.setPrimitiveType(uint8_t(5));
    return;
  }
  int num = symbol.getTypeNum();
  std::vector<uint64_t> subdim(
      dim.begin() + p_variable_ref.getChildren().size(), dim.end());

  p_variable_ref.ptype.setPrimitiveType(num, subdim);
  // constant.clear();
}

void SemanticAnalyzer::visit(AssignmentNode &p_assignment) {
  /*
   * TODO:
   *
   * 1. Push a new symbol table if this node forms a scope.
   * 2. Insert the symbol into current symbol table if this node is related to
   *    declaration (ProgramNode, VariableNode, FunctionNode).
   * 3. Travere child nodes of this node.
   * 4. Perform semantic analyses of this node.
   * 5. Pop the symbol table pushed at the 1st step.
   */
  p_assignment.visitChildNodes(*this);
  // TODO assign the type to left value from right value
  auto leftType = p_assignment.getLeft()->ptype;
  auto rightType = p_assignment.getRight()->ptype;
  std::string leftName = p_assignment.getLeft()->getNameCString();
  if (leftType.isError())
    return;

  if (!leftType.isScalar()) {
    errorMessage->errorAssignArrayType(
        p_assignment.getLeft()->getLocation().line,
        p_assignment.getLeft()->getLocation().col);
    return;
  }
  if (!symbolManager.getSymbol(leftName).getKind().compare("constant")) {
    errorMessage->errorAssignConstType(
        p_assignment.getLeft()->getLocation().line,
        p_assignment.getLeft()->getLocation().col, leftName);
    return;
  }
  if (!symbolManager.getLastSybole()->getSymbol(leftName).getKind().compare(
          "loop_var") &&
      !symbolManager.getLastSybole()->getContext().compare("for")) {

    errorMessage->errorAssignVarRefLoop(
        p_assignment.getLeft()->getLocation().line,
        p_assignment.getLeft()->getLocation().col);
    return; // bug
  }
  if (!rightType.isScalar()) {
    errorMessage->errorAssignResult(p_assignment.getRight()->getLocation().line,
                                    p_assignment.getRight()->getLocation().col);
    return;
  }
  if (!leftType.compare(rightType)) {
    errorMessage->errorAssignLvalue(
        p_assignment.getLocation().line, p_assignment.getLocation().col,
        leftType.getPTypeCString(), rightType.getPTypeCString());
    return;
  }
}

void SemanticAnalyzer::visit(ReadNode &p_read) {
  /*
   * TODO:
   *
   * 1. Push a new symbol table if this node forms a scope.
   * 2. Insert the symbol into current symbol table if this node is related to
   *    declaration (ProgramNode, VariableNode, FunctionNode).
   * 3. Travere child nodes of this node.
   * 4. Perform semantic analyses of this node.
   * 5. Pop the symbol table pushed at the 1st step.
   */
  p_read.visitChildNodes(*this);
  auto childType = p_read.getChildren()->ptype;
  if (!childType.isScalar()) {
    errorMessage->errorReadType(p_read.getChildren()->getLocation().line,
                                p_read.getChildren()->getLocation().col);
    return;
  }
  std::string childName = p_read.getChildren()->getNameCString();
  auto symbol = symbolManager.getSymbol(childName);
  if (!symbol.getKind().compare("loop_var") ||
      !symbol.getKind().compare("constant")) {
    errorMessage->errorReadVar(p_read.getChildren()->getLocation().line,
                               p_read.getChildren()->getLocation().col);
  }
}

void SemanticAnalyzer::visit(IfNode &p_if) {
  /*
   * TODO:
   *
   * 1. Push a new symbol table if this node forms a scope.
   * 2. Insert the symbol into current symbol table if this node is related to
   *    declaration (ProgramNode, VariableNode, FunctionNode).
   * 3. Travere child nodes of this node.
   * 4. Perform semantic analyses of this node.
   * 5. Pop the symbol table pushed at the 1st step.
   */
  p_if.visitChildNodes(*this);
  auto conditionType = p_if.getCondition()->ptype;
  if (conditionType.isError())
    return;
  if (!conditionType.isBool()) {
    errorMessage->errorIFWhileResult(p_if.getCondition()->getLocation().line,
                                     p_if.getCondition()->getLocation().col);
    return;
  }
}

void SemanticAnalyzer::visit(WhileNode &p_while) {
  /*
   * TODO:
   *
   * 1. Push a new symbol table if this node forms a scope.
   * 2. Insert the symbol into current symbol table if this node is related to
   *    declaration (ProgramNode, VariableNode, FunctionNode).
   * 3. Travere child nodes of this node.
   * 4. Perform semantic analyses of this node.
   * 5. Pop the symbol table pushed at the 1st step.
   */
  p_while.visitChildNodes(*this);
  auto conditionType = p_while.getCondition()->ptype;
  if (conditionType.isError())
    return;
  if (!conditionType.isBool()) {
    errorMessage->errorIFWhileResult(p_while.getCondition()->getLocation().line,
                                     p_while.getCondition()->getLocation().col);
    return;
  }
}

void SemanticAnalyzer::visit(ForNode &p_for) {
  /*
   * TODO:
   *
   * 1. Push a new symbol table if this node forms a scope.
   * 2. Insert the symbol into current symbol table if this node is related to
   *    declaration (ProgramNode, VariableNode, FunctionNode).
   * 3. Travere child nodes of this node.
   * 4. Perform semantic analyses of this node.
   * 5. Pop the symbol table pushed at the 1st step.
   */
  auto symbolTable = new SymbolTable("for");
  symbolManager.pushScope(symbolTable);
  forPath = 1;
  p_for.visitChildNodes(*this);
  // TO
  std::string initValue =
      p_for.getInitStmt()->getRight()->getConstantValueCString();
  std::string endValue = p_for.getEndCond()->getConstantValueCString();
  int end = std::atoi(endValue.c_str()), init = std::atoi(initValue.c_str());
  if (end < init) {
    errorMessage->errorForInitial(p_for.getLocation().line,
                                  p_for.getLocation().col);
  }

  TABLEDUMP symbolManager.popScope();
  forPath = 0;
}

void SemanticAnalyzer::visit(ReturnNode &p_return) {
  /*
   * TODO:
   *
   * 1. Push a new symbol table if this node forms a scope.
   * 2. Insert the symbol into current symbol table if this node is related to
   *    declaration (ProgramNode, VariableNode, FunctionNode).
   * 3. Travere child nodes of this node.
   * 4. Perform semantic analyses of this node.
   * 5. Pop the symbol table pushed at the 1st step.
   */
  p_return.visitChildNodes(*this);
  auto returnType = p_return.getRetVal()->ptype;
  if (returnType.isError())
    return;
  // printf("--%d----%s\n", p_return.getRetVal()->getLocation().line,
  //        symbolManager.getSybolTable()->getContext().c_str());
  if (!symbolManager.getLastSybole()->getContext().compare("program") &&
      !symbolManager.getSybolTable()->getContext().compare("compound")) {
    errorMessage->errorReturnVoid(p_return.getLocation().line,
                                  p_return.getLocation().col);
    return;
  }
  std::string funcName;
  for (size_t j = 0; j < symbolManager.getTable().size(); j++) {
    // auto symbolTable = symbolManager.getTable()[j]->getSymbolTableList();
    auto symbolTableList = symbolManager.getLastSybole();
    auto symbolTable = symbolTableList->getSymbolTableList();
    for (size_t i = 0; i < symbolTable.size(); i++) {
      if (!symbolTable[i].getKind().compare("function")) {
        funcName = symbolTable[i].getName();
      }
    }
  }
  if (funcName.empty())
    return;
  // printf("----'%s'--%d-\n", funcName.c_str(),
  //        symbolManager.getSymbol(funcName).getTypeNum());
  // printf("---%s----\n", p_return.getRetVal()->ptype.getPTypeCString());
  uint8_t functionType = symbolManager.getSymbol(funcName).getTypeNum();
  PType funcPType(functionType, symbolManager.getSymbol(funcName).getDim());
  // printf("-----%s----\n", funcPType.getPTypeCString());
  if (!returnType.compare(funcPType)) {
    errorMessage->errorReturnType(p_return.getRetVal()->getLocation().line,
                                  p_return.getRetVal()->getLocation().col,
                                  returnType.getPTypeCString(),
                                  funcPType.getPTypeCString());
    return;
  }
}
