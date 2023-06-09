#ifndef AST_P_TYPE_H
#define AST_P_TYPE_H

#include <memory>
#include <string>
#include <vector>

class PType;

using PTypeSharedPtr = std::shared_ptr<PType>;

class PType {
public:
  enum class PrimitiveTypeEnum : uint8_t {
    kVoidType,
    kIntegerType,
    kRealType,
    kBoolType,
    kStringType,
    kErrorType
  };

private:
  PrimitiveTypeEnum m_type;
  std::vector<uint64_t> m_dimensions;
  mutable std::string m_type_string;
  mutable bool m_type_string_is_valid = false;

public:
  ~PType() = default;
  PType(const PrimitiveTypeEnum type) : m_type(type) {}
  PType(uint8_t type, std::vector<uint64_t> &dim) {
    m_type = static_cast<PrimitiveTypeEnum>(type);
    m_dimensions = dim;
  }

  void setDimensions(std::vector<uint64_t> &p_dims) {
    m_dimensions = std::move(p_dims);
  }

  PrimitiveTypeEnum getPrimitiveType() const { return m_type; }
  void setPrimitiveType(const PType &other) {
    m_type = other.m_type;
    m_dimensions = other.m_dimensions;
  }
  void setPrimitiveType(const PrimitiveTypeEnum type,
                        std::vector<uint64_t> &dim) {
    m_type = type;
    m_dimensions = dim;
  }
  void setPrimitiveType(uint8_t type, std::vector<uint64_t> &dim) {
    m_type = static_cast<PrimitiveTypeEnum>(type);
    m_dimensions = dim;
  }
  void setPrimitiveType(uint8_t type) {
    m_type = static_cast<PrimitiveTypeEnum>(type);
  }
  bool isVoid() const { return m_type == PrimitiveTypeEnum::kVoidType; }
  bool isString() const {
    return m_type == PrimitiveTypeEnum::kStringType && m_dimensions.empty();
  }
  bool isReal() const {
    return m_type == PrimitiveTypeEnum::kRealType && m_dimensions.empty();
  }
  bool isInt() const {
    return m_type == PrimitiveTypeEnum::kIntegerType && m_dimensions.empty();
  }
  bool isBool() const {
    return m_type == PrimitiveTypeEnum::kBoolType && m_dimensions.empty();
  }
  bool isScalar() const {
    return m_dimensions.empty() && m_type != PrimitiveTypeEnum::kVoidType;
  }
  bool isError() const { return m_type == PrimitiveTypeEnum::kErrorType; }
  bool compare(PType &other) const {
    // handle casting
    if (static_cast<size_t>(m_type) < 3 && static_cast<size_t>(m_type) > 0 &&
        static_cast<size_t>(other.m_type) < 3 &&
        static_cast<size_t>(other.m_type) > 0) {
      return m_dimensions == other.m_dimensions;
    }
    return m_type == other.m_type && m_dimensions == other.m_dimensions;
  }
  bool compareMod(PType &other) const {
    return m_type == other.m_type && m_dimensions == other.m_dimensions;
  }
  PType &wilderType(PType &other) {
    // handle casting
    if (static_cast<size_t>(m_type) < 3 && static_cast<size_t>(m_type) > 0 &&
        static_cast<size_t>(other.m_type) < 3 &&
        static_cast<size_t>(other.m_type) > 0) {
      if (static_cast<size_t>(m_type) > static_cast<size_t>(other.m_type)) {
        return *this;
      } else {
        return other;
      }
    }
    return *this;
  }
  const char *getPTypeCString() const;

  std::vector<uint64_t> getDimensions() { return m_dimensions; }
};

#endif
