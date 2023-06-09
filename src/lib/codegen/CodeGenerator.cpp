#include "codegen/CodeGenerator.hpp"
#include "visitor/AstNodeInclude.hpp"

#include <algorithm>
#include <cassert>
#include <cstdarg>
#include <cstdio>
int currentOffset = -12;
int conditionLabel = 1;
CodeGenerator::CodeGenerator(const std::string source_file_name,
                             const std::string save_path,
                             const SymbolManager *const p_symbol_manager)
    : m_symbol_manager_ptr(p_symbol_manager),
      m_source_file_path(source_file_name) {
  // FIXME: assume that the source file is always xxxx.p
  const std::string &real_path =
      (save_path == "") ? std::string{"."} : save_path;
  auto slash_pos = source_file_name.rfind("/");
  auto dot_pos = source_file_name.rfind(".");

  if (slash_pos != std::string::npos) {
    ++slash_pos;
  } else {
    slash_pos = 0;
  }
  std::string output_file_path(
      real_path + "/" +
      source_file_name.substr(slash_pos, dot_pos - slash_pos) + ".S");
  m_output_file.reset(fopen(output_file_path.c_str(), "w"));
  assert(m_output_file.get() && "Failed to open output file");
}

static void dumpInstructions(FILE *p_out_file, const char *format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf(p_out_file, format, args);
  va_end(args);
}

void CodeGenerator::visit(ProgramNode &p_program) {
  // Generate RISC-V instructions for program header
  // clang-format off
    constexpr const char*const riscv_assembly_file_prologue =
        "    .file \"%s\"\n"
        "    .option nopic\n"
        ".section    .text\n"
        "    .align 2\n";
  // clang-format on
  dumpInstructions(m_output_file.get(), riscv_assembly_file_prologue,
                   m_source_file_path.c_str());

  // Reconstruct the hash table for looking up the symbol entry
  // Hint: Use symbol_manager->lookup(symbol_name) to get the symbol entry.
  m_symbol_manager_ptr->reconstructHashTableFromSymbolTable(
      p_program.getSymbolTable());

  auto visit_ast_node = [&](auto &ast_node) { ast_node->accept(*this); };
  for_each(p_program.getDeclNodes().begin(), p_program.getDeclNodes().end(),
           visit_ast_node);
  for_each(p_program.getFuncNodes().begin(), p_program.getFuncNodes().end(),
           visit_ast_node);
  constexpr const char *const compund_statement_prologue =
      "    .text\n"
      "    .align	2\n"
      "    .globl	main\n"
      "    .type	main, @function\n"
      "main:\n"
      "    addi	sp, sp, -128\n"
      "    sw ra, 124(sp)\n"
      "    sw s0, 120(sp)\n"
      "    addi s0, sp, 128\n";
  dumpInstructions(m_output_file.get(), compund_statement_prologue);
  currentOffset = -12;
  const_cast<CompoundStatementNode &>(p_program.getBody()).accept(*this);
  constexpr const char *const compund_statement_epilogue =
      "    lw ra, 124(sp)\n"
      "    lw s0, 120(sp)\n"
      "    addi sp, sp, 128\n"
      "    jr ra\n"
      "    .size main, .-main\n";
  // clang-format on
  dumpInstructions(m_output_file.get(), compund_statement_epilogue);

  // Remove the entries in the hash table
  m_symbol_manager_ptr->removeSymbolsFromHashTable(p_program.getSymbolTable());
}

void CodeGenerator::visit(DeclNode &p_decl) {
  p_decl.visitChildNodes(*this);
  for (size_t i = 0; i < p_decl.getVariables().size(); i++) {
    std::string varName = p_decl.getVariables()[i]->getName();
    auto symbol = m_symbol_manager_ptr->lookup(varName);
    auto kind = symbol->getKind();
    auto level = symbol->getLevel();
    switch (kind) {
    case SymbolEntry::KindEnum::kVariableKind: {
      if (level == 0) {
        constexpr const char *const rsicvAsm = "    .comm	%s,%d,%d\n";
        dumpInstructions(m_output_file.get(), rsicvAsm, varName.c_str(), 4, 4);
      } else {
        const_cast<SymbolEntry *>(symbol)->setOffset(currentOffset);
        currentOffset -= 4;
      }
      break;
    }
    case SymbolEntry::KindEnum::kConstantKind: {
      std::string attr =
          symbol->getAttribute().constant()->getConstantValueCString();
      if (level == 0) {

        constexpr const char *const rsicvAsm = ".section    .rodata \n"
                                               "    .align 2\n"
                                               "    .globl %s\n"
                                               "    .type %s, @object\n"
                                               "    .size %s, %d\n"
                                               "%s:\n"
                                               "    .word %d\n";
        dumpInstructions(m_output_file.get(), rsicvAsm, varName.c_str(),
                         varName.c_str(), varName.c_str(), 4, varName.c_str(),
                         std::stoi(attr));
      } else {
        const_cast<SymbolEntry *>(symbol)->setOffset(currentOffset);
        currentOffset -= 4;
        constexpr const char *const rsicvAsm = "     addi t0, s0, %d\n"
                                               "     addi sp, sp, -4\n"
                                               "     sw t0, 0(sp)\n"
                                               "     li t0, %s\n"
                                               "     addi sp, sp, -4\n"
                                               "     sw t0, 0(sp)\n"
                                               "     lw t0, 0(sp)\n"
                                               "     addi sp, sp, 4\n"
                                               "     lw t1, 0(sp)\n"
                                               "     addi sp, sp, 4\n"
                                               "     sw t0, 0(t1)\n";
        dumpInstructions(m_output_file.get(), rsicvAsm, symbol->getOffset(),
                         attr.c_str());
      }
      break;
    }
    case SymbolEntry::KindEnum::kLoopVarKind: {
      const_cast<SymbolEntry *>(symbol)->setOffset(currentOffset);
      currentOffset -= 4;
      break;
    }
    default:
      break;
    }
  }
}

void CodeGenerator::visit(VariableNode &p_variable) {
  //   p_variable.visitChildNodes(*this);
}

void CodeGenerator::visit(ConstantValueNode &p_constant_value) {
  constexpr const char *const rsicvAsm = "    li t0, %s\n"
                                         "    addi sp, sp, -4\n"
                                         "    sw t0, 0(sp)\n";
  dumpInstructions(m_output_file.get(), rsicvAsm,
                   p_constant_value.getConstantValueCString());
}

void CodeGenerator::visit(FunctionNode &p_function) {
  // Reconstruct the hash table for looking up the symbol entry
  m_symbol_manager_ptr->reconstructHashTableFromSymbolTable(
      p_function.getSymbolTable());
  constexpr const char *const compund_function_prologue =
      ".section    .text\n"
      "    .align	2\n"
      "    .globl	%s\n"
      "    .type	%s, @function\n"
      "%s:\n"
      "    addi	sp, sp, -128\n"
      "    sw ra, 124(sp)\n"
      "    sw s0, 120(sp)\n"
      "    addi s0, sp, 128\n";
  dumpInstructions(m_output_file.get(), compund_function_prologue,
                   p_function.getNameCString(), p_function.getNameCString(),
                   p_function.getNameCString());
  currentOffset = -12;
  // parameter
  for (size_t i = 0; i < p_function.getParameters().size(); i++) {
    for (size_t j = 0; j < p_function.getParameters()[i]->getVariables().size();
         j++) {
      auto varName =
          p_function.getParameters()[i]->getVariables()[j]->getName();
      auto symbol = m_symbol_manager_ptr->lookup(varName);
      currentOffset = currentOffset + (-4 * (i + j));
      const_cast<SymbolEntry *>(symbol)->setOffset(currentOffset);
      constexpr const char *const rsicvAsm = "    sw a%d, %d(s0)\n";
      dumpInstructions(m_output_file.get(), rsicvAsm, i + j,
                       symbol->getOffset());
    }
  }
  currentOffset -= 4;
  p_function.getBody()->accept(*this);
  constexpr const char *const compund_function_epilogue =
      "    lw ra, 124(sp)\n"
      "    lw s0, 120(sp)\n"
      "    addi sp, sp, 128\n"
      "    jr ra\n"
      "    .size %s, .-%s\n";
  // clang-format on
  dumpInstructions(m_output_file.get(), compund_function_epilogue,
                   p_function.getNameCString(), p_function.getNameCString());

  // Remove the entries in the hash table
  m_symbol_manager_ptr->removeSymbolsFromHashTable(p_function.getSymbolTable());
}

void CodeGenerator::visit(CompoundStatementNode &p_compound_statement) {
  // Reconstruct the hash table for looking up the symbol entry
  m_symbol_manager_ptr->reconstructHashTableFromSymbolTable(
      p_compound_statement.getSymbolTable());

  p_compound_statement.visitChildNodes(*this);

  // Remove the entries in the hash table
  m_symbol_manager_ptr->removeSymbolsFromHashTable(
      p_compound_statement.getSymbolTable());
}

void CodeGenerator::visit(PrintNode &p_print) {

  p_print.visitChildNodes(*this);

  constexpr const char *const rsicvAsm = "    lw a0, 0(sp)\n"
                                         "    addi sp, sp, 4\n"
                                         "    jal ra, printInt\n";
  dumpInstructions(m_output_file.get(), rsicvAsm);
}

void CodeGenerator::visit(BinaryOperatorNode &p_bin_op) {
  p_bin_op.visitChildNodes(*this);

  std::string op = p_bin_op.getOpCString();
  if (!op.compare("<=")) {
    constexpr const char *const rsicvAsm = "    lw t0, 0(sp)\n"
                                           "    addi sp, sp, 4\n"
                                           "    lw t1, 0(sp)\n"
                                           "    addi sp, sp, 4\n"
                                           "    bgt t1, t0, L%d\n";
    dumpInstructions(m_output_file.get(), rsicvAsm, conditionLabel + 1);
  }
  if (!op.compare("<")) {
    constexpr const char *const rsicvAsm = "    lw t0, 0(sp)\n"
                                           "    addi sp, sp, 4\n"
                                           "    lw t1, 0(sp)\n"
                                           "    addi sp, sp, 4\n"
                                           "    bge t1, t0, L%d\n";
    dumpInstructions(m_output_file.get(), rsicvAsm, conditionLabel + 1);
  }
  if (!op.compare("=")) {
    constexpr const char *const rsicvAsm = "    lw t0, 0(sp)\n"
                                           "    addi sp, sp, 4\n"
                                           "    lw t1, 0(sp)\n"
                                           "    addi sp, sp, 4\n"
                                           "    bne t1, t0, L%d\n";
    dumpInstructions(m_output_file.get(), rsicvAsm, conditionLabel + 1);
  }
  if (!op.compare("!=")) {
    constexpr const char *const rsicvAsm = "    lw t0, 0(sp)\n"
                                           "    addi sp, sp, 4\n"
                                           "    lw t1, 0(sp)\n"
                                           "    addi sp, sp, 4\n"
                                           "    beq t1, t0, L%d\n";
    dumpInstructions(m_output_file.get(), rsicvAsm, conditionLabel + 1);
  }
  if (!op.compare(">=")) {
    constexpr const char *const rsicvAsm = "    lw t0, 0(sp)\n"
                                           "    addi sp, sp, 4\n"
                                           "    lw t1, 0(sp)\n"
                                           "    addi sp, sp, 4\n"
                                           "    blt t1, t0, L%d\n";
    dumpInstructions(m_output_file.get(), rsicvAsm, conditionLabel + 1);
  }
  if (!op.compare(">")) {
    constexpr const char *const rsicvAsm = "    lw t0, 0(sp)\n"
                                           "    addi sp, sp, 4\n"
                                           "    lw t1, 0(sp)\n"
                                           "    addi sp, sp, 4\n"
                                           "    ble t1, t0, L%d\n";
    dumpInstructions(m_output_file.get(), rsicvAsm, conditionLabel + 1);
  }

  if (!op.compare("+")) {
    constexpr const char *const rsicvAsm = "    lw t0, 0(sp)\n"
                                           "    addi sp, sp, 4\n"
                                           "    lw t1, 0(sp)\n"
                                           "    addi sp, sp, 4\n"
                                           "    add t0, t1, t0\n"
                                           "    addi sp, sp, -4\n"
                                           "    sw t0, 0(sp)\n";
    dumpInstructions(m_output_file.get(), rsicvAsm);
  }
  if (!op.compare("*")) {
    constexpr const char *const rsicvAsm = "    lw t0, 0(sp)\n"
                                           "    addi sp, sp, 4\n"
                                           "    lw t1, 0(sp)\n"
                                           "    addi sp, sp, 4\n"
                                           "    mul t0, t1, t0\n"
                                           "    addi sp, sp, -4\n"
                                           "    sw t0, 0(sp)\n";
    dumpInstructions(m_output_file.get(), rsicvAsm);
  }
  if (!op.compare("-")) {
    constexpr const char *const rsicvAsm = "    lw t0, 0(sp)\n"
                                           "    addi sp, sp, 4\n"
                                           "    lw t1, 0(sp)\n"
                                           "    addi sp, sp, 4\n"
                                           "    sub t0, t1, t0\n"
                                           "    addi sp, sp, -4\n"
                                           "    sw t0, 0(sp)\n";
    dumpInstructions(m_output_file.get(), rsicvAsm);
  }
  if (!op.compare("/")) {
    constexpr const char *const rsicvAsm = "    lw t0, 0(sp)\n"
                                           "    addi sp, sp, 4\n"
                                           "    lw t1, 0(sp)\n"
                                           "    addi sp, sp, 4\n"
                                           "    div t0, t1, t0\n"
                                           "    addi sp, sp, -4\n"
                                           "    sw t0, 0(sp)\n";
    dumpInstructions(m_output_file.get(), rsicvAsm);
  }
  if (!op.compare("mod")) {
    constexpr const char *const rsicvAsm = "    lw t0, 0(sp)\n"
                                           "    addi sp, sp, 4\n"
                                           "    lw t1, 0(sp)\n"
                                           "    addi sp, sp, 4\n"
                                           "    rem t0, t1, t0\n"
                                           "    addi sp, sp, -4\n"
                                           "    sw t0, 0(sp)\n";
    dumpInstructions(m_output_file.get(), rsicvAsm);
  }
}

void CodeGenerator::visit(UnaryOperatorNode &p_un_op) {}

void CodeGenerator::visit(FunctionInvocationNode &p_func_invocation) {
  p_func_invocation.visitChildNodes(*this);
  int argNum = p_func_invocation.getArguments().size();
  for (int i = 0; i < argNum; i++) {
    constexpr const char *const rsicvAsm = "    lw a%d, 0(sp)\n"
                                           "    addi sp, sp, 4\n";
    dumpInstructions(m_output_file.get(), rsicvAsm, argNum - (i + 1));
  }
  constexpr const char *const rsicvAsm = "    jal ra, %s\n"
                                         "    mv t0, a0\n"
                                         "    addi sp, sp, -4\n"
                                         "    sw t0, 0(sp)\n";
  dumpInstructions(m_output_file.get(), rsicvAsm,
                   p_func_invocation.getNameCString());
}

void CodeGenerator::visit(VariableReferenceNode &p_variable_ref) {
  auto symbol = m_symbol_manager_ptr->lookup(p_variable_ref.getName());
  auto level = symbol->getLevel();
  if (level == 0) {
    constexpr const char *const rsicvAsm = "    la t0, %s\n"
                                           "    lw t1, 0(t0)\n"
                                           "    mv t0, t1\n"
                                           "    addi sp, sp, -4\n"
                                           "    sw t0, 0(sp)\n";
    dumpInstructions(m_output_file.get(), rsicvAsm,
                     p_variable_ref.getNameCString());
  } else {
    constexpr const char *const rsicvAsm = "    lw t0, %d(s0)\n"
                                           "    addi sp, sp, -4\n"
                                           "    sw t0, 0(sp)\n";
    dumpInstructions(m_output_file.get(), rsicvAsm, symbol->getOffset());
  }
}

void CodeGenerator::visit(AssignmentNode &p_assignment) {
  // left
  std::string varRefName = p_assignment.getLvalue().getName();
  auto symbol = m_symbol_manager_ptr->lookup(varRefName);
  auto level = symbol->getLevel();
  if (level == 0) {
    constexpr const char *const rsicvAsm = "    la t0, %s\n"
                                           "    addi sp, sp, -4\n"
                                           "    sw t0, 0(sp)\n";
    dumpInstructions(m_output_file.get(), rsicvAsm, varRefName.c_str());
  } else {
    constexpr const char *const rsicvAsm = "    addi t0, s0, %d\n"
                                           "    addi sp, sp, -4\n"
                                           "    sw t0, 0(sp)\n";
    dumpInstructions(m_output_file.get(), rsicvAsm, symbol->getOffset());
  }
  // visit expression
  const_cast<ExpressionNode &>(p_assignment.getExpr()).accept(*this);

  constexpr const char *const rsicvAsm = "    lw t0, 0(sp)\n"
                                         "    addi sp, sp, 4\n"
                                         "    lw t1, 0(sp)\n"
                                         "    addi sp, sp, 4\n"
                                         "    sw t0, 0(t1)\n";
  dumpInstructions(m_output_file.get(), rsicvAsm);
}

void CodeGenerator::visit(ReadNode &p_read) {
  std::string targetNmae = p_read.getTarget().getName();
  auto symbol = m_symbol_manager_ptr->lookup(targetNmae);
  auto level = symbol->getLevel();
  if (level == 0) {
    constexpr const char *const rsicvAsm = "    la t0, %s\n"
                                           "    addi sp, sp, -4\n"
                                           "    sw t0, 0(sp)\n";
    dumpInstructions(m_output_file.get(), rsicvAsm, targetNmae.c_str());
  } else {
    constexpr const char *const rsicvAsm = "    addi t0, s0, %d\n"
                                           "    addi sp, sp, -4\n"
                                           "    sw t0, 0(sp)\n";
    dumpInstructions(m_output_file.get(), rsicvAsm, symbol->getOffset());
  }
  constexpr const char *const rsicvAsm = "    jal ra, readInt\n"
                                         "    lw t0, 0(sp)\n"
                                         "    addi sp, sp, 4\n"
                                         "    sw a0, 0(t0)\n";
  dumpInstructions(m_output_file.get(), rsicvAsm, symbol->getOffset());
}

void CodeGenerator::visit(IfNode &p_if) {
  const_cast<ExpressionNode &>(p_if.getCondition()).accept(*this);
  constexpr const char *const rsicvAsm = "L%d:\n";
  dumpInstructions(m_output_file.get(), rsicvAsm, conditionLabel);
  conditionLabel += 1;
  const_cast<CompoundStatementNode &>(p_if.getMybody()).accept(*this);
  constexpr const char *const rsicvAsm1 = "     j L%d\n"
                                          "L%d:\n";
  dumpInstructions(m_output_file.get(), rsicvAsm1, conditionLabel + 1,
                   conditionLabel);
  conditionLabel += 1;
  const_cast<CompoundStatementNode &>(p_if.getElseBody()).accept(*this);
  constexpr const char *const rsicvAsm2 = "L%d:\n";
  dumpInstructions(m_output_file.get(), rsicvAsm2, conditionLabel);
  conditionLabel += 1;
}

void CodeGenerator::visit(WhileNode &p_while) {
  constexpr const char *const rsicvAsm = "L%d:\n";
  dumpInstructions(m_output_file.get(), rsicvAsm, conditionLabel);
  conditionLabel += 1;
  const_cast<ExpressionNode &>(p_while.getCondition()).accept(*this);
  constexpr const char *const rsicvAsm1 = "L%d:\n";
  dumpInstructions(m_output_file.get(), rsicvAsm1, conditionLabel,
                   conditionLabel);
  const_cast<CompoundStatementNode &>(p_while.getbody()).accept(*this);
  constexpr const char *const rsicvAsm2 = "     j L%d\n"
                                          "L%d:\n";
  dumpInstructions(m_output_file.get(), rsicvAsm2, conditionLabel - 1,
                   conditionLabel + 1);
  conditionLabel += 2;
}

void CodeGenerator::visit(ForNode &p_for) {

  // Reconstruct the hash table for looking up the symbol entry
  m_symbol_manager_ptr->reconstructHashTableFromSymbolTable(
      p_for.getSymbolTable());
  p_for.getLoopVarDecl()->accept(*this);
  p_for.getAssign()->accept(*this);
  std::string var =
      const_cast<VariableReferenceNode &>(p_for.getAssign()->getLvalue())
          .getName();
  auto symbol = m_symbol_manager_ptr->lookup(var);
  constexpr const char *const rsicvAsm = "L%d:\n"
                                         "     lw t0, %d(s0)\n"
                                         "     addi sp, sp, -4\n"
                                         "     sw t0, 0(sp)\n";
  dumpInstructions(m_output_file.get(), rsicvAsm, conditionLabel,
                   symbol->getOffset());
  p_for.getEndCOndition()->accept(*this);
  constexpr const char *const rsicvAsm1 = "     lw t0, 0(sp)\n"
                                          "     addi sp, sp, 4\n"
                                          "     lw t1, 0(sp)\n"
                                          "     addi sp, sp, 4\n"
                                          "     bge t1, t0, L%d\n"
                                          "L%d:\n";
  dumpInstructions(m_output_file.get(), rsicvAsm1, conditionLabel + 2,
                   conditionLabel + 1);
  conditionLabel += 2;
  int label = conditionLabel;
  conditionLabel += 1;
  p_for.getBody()->accept(*this);
  constexpr const char *const rsicvAsm2 = "    addi t0, s0, %d\n"
                                          "    addi sp, sp, -4\n"
                                          "    sw t0, 0(sp)\n"
                                          "    lw t0, %d(s0)\n"
                                          "    addi sp, sp, -4\n"
                                          "    sw t0, 0(sp)\n"
                                          "    li t0, 1\n"
                                          "    addi sp, sp, -4\n"
                                          "    sw t0, 0(sp)\n"
                                          "    lw t0, 0(sp)\n"
                                          "    addi sp, sp, 4\n"
                                          "    lw t1, 0(sp)\n"
                                          "    addi sp, sp, 4\n"
                                          "    add t0, t1, t0\n"
                                          "    addi sp, sp, -4\n"
                                          "    sw t0, 0(sp)\n"
                                          "    lw t0, 0(sp)\n"
                                          "    addi sp, sp, 4\n"
                                          "    lw t1, 0(sp)\n"
                                          "    addi sp, sp, 4\n"
                                          "    sw t0, 0(t1)\n"
                                          "    j L%d\n"
                                          "L%d:\n";
  dumpInstructions(m_output_file.get(), rsicvAsm2, symbol->getOffset(),
                   symbol->getOffset(), label - 2, label);
  // Remove the entries in the hash table
  m_symbol_manager_ptr->removeSymbolsFromHashTable(p_for.getSymbolTable());
}

void CodeGenerator::visit(ReturnNode &p_return) {
  p_return.visitChildNodes(*this);

  constexpr const char *const rsicvAsm = "    lw t0, 0(sp)\n"
                                         "    addi sp, sp, 4\n"
                                         "    mv a0, t0\n";
  dumpInstructions(m_output_file.get(), rsicvAsm);
}
