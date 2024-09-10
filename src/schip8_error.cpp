#include "schip8_error.hpp"

namespace SuperChip8 {

static const ErrorCategory errorCategoryInstance;

const ::std::error_category& category() {
  return errorCategoryInstance;
}

}  // namespace SuperChip8