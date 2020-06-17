// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

// Eager evaluation convenience APIs for invoking common functions, including
// necessary memory allocations

#pragma once

#include <string>
#include <utility>

#include "arrow/compute/exec.h"  // IWYU pragma: keep
#include "arrow/compute/function.h"
#include "arrow/datum.h"
#include "arrow/result.h"
#include "arrow/util/macros.h"
#include "arrow/util/visibility.h"

namespace arrow {
namespace compute {

// ----------------------------------------------------------------------

/// \brief Add two values together. Array values must be the same length. If
/// either addend is null the result will be null.
///
/// \param[in] left the first addend
/// \param[in] right the second addend
/// \param[in] ctx the function execution context, optional
/// \return the elementwise sum
ARROW_EXPORT
Result<Datum> Add(const Datum& left, const Datum& right, ExecContext* ctx = NULLPTR);

/// \brief Subtract two values. Array values must be the same length. If the
/// minuend or subtrahend is null the result will be null.
///
/// \param[in] left the value subtracted from (minuend)
/// \param[in] right the value by which the minuend is reduced (subtrahend)
/// \param[in] ctx the function execution context, optional
/// \return the elementwise difference
ARROW_EXPORT
Result<Datum> Subtract(const Datum& left, const Datum& right, ExecContext* ctx = NULLPTR);

/// \brief Multiply two values. Array values must be the same length. If either
/// factor is null the result will be null.
///
/// \param[in] left the first factor
/// \param[in] right the second factor
/// \param[in] ctx the function execution context, optional
/// \return the elementwise product
ARROW_EXPORT
Result<Datum> Multiply(const Datum& left, const Datum& right, ExecContext* ctx = NULLPTR);

enum CompareOperator {
  EQUAL,
  NOT_EQUAL,
  GREATER,
  GREATER_EQUAL,
  LESS,
  LESS_EQUAL,
};

struct CompareOptions : public FunctionOptions {
  explicit CompareOptions(CompareOperator op) : op(op) {}

  enum CompareOperator op;
};

/// \brief Compare a numeric array with a scalar.
///
/// \param[in] left datum to compare, must be an Array
/// \param[in] right datum to compare, must be a Scalar of the same type than
///            left Datum.
/// \param[in] options compare options
/// \param[in] ctx the function execution context, optional
/// \return resulting datum
///
/// Note on floating point arrays, this uses ieee-754 compare semantics.
///
/// \since 1.0.0
/// \note API not yet finalized
ARROW_EXPORT
Result<Datum> Compare(const Datum& left, const Datum& right,
                      struct CompareOptions options, ExecContext* ctx = NULLPTR);

/// \brief Invert the values of a boolean datum
/// \param[in] value datum to invert
/// \param[in] ctx the function execution context, optional
/// \return the resulting datum
///
/// \since 1.0.0
/// \note API not yet finalized
ARROW_EXPORT
Result<Datum> Invert(const Datum& value, ExecContext* ctx = NULLPTR);

/// \brief Element-wise AND of two boolean datums which always propagates nulls
/// (null and false is null).
///
/// \param[in] left left operand (array)
/// \param[in] right right operand (array)
/// \param[in] ctx the function execution context, optional
/// \return the resulting datum
///
/// \since 1.0.0
/// \note API not yet finalized
ARROW_EXPORT
Result<Datum> And(const Datum& left, const Datum& right, ExecContext* ctx = NULLPTR);

/// \brief Element-wise AND of two boolean datums with a Kleene truth table
/// (null and false is false).
///
/// \param[in] left left operand (array)
/// \param[in] right right operand (array)
/// \param[in] ctx the function execution context, optional
/// \return the resulting datum
///
/// \since 1.0.0
/// \note API not yet finalized
ARROW_EXPORT
Result<Datum> KleeneAnd(const Datum& left, const Datum& right,
                        ExecContext* ctx = NULLPTR);

/// \brief Element-wise OR of two boolean datums which always propagates nulls
/// (null and true is null).
///
/// \param[in] left left operand (array)
/// \param[in] right right operand (array)
/// \param[in] ctx the function execution context, optional
/// \return the resulting datum
///
/// \since 1.0.0
/// \note API not yet finalized
ARROW_EXPORT
Result<Datum> Or(const Datum& left, const Datum& right, ExecContext* ctx = NULLPTR);

/// \brief Element-wise OR of two boolean datums with a Kleene truth table
/// (null or true is true).
///
/// \param[in] left left operand (array)
/// \param[in] right right operand (array)
/// \param[in] ctx the function execution context, optional
/// \return the resulting datum
///
/// \since 1.0.0
/// \note API not yet finalized
ARROW_EXPORT
Result<Datum> KleeneOr(const Datum& left, const Datum& right, ExecContext* ctx = NULLPTR);

/// \brief Element-wise XOR of two boolean datums
/// \param[in] left left operand (array)
/// \param[in] right right operand (array)
/// \param[in] ctx the function execution context, optional
/// \return the resulting datum
///
/// \since 1.0.0
/// \note API not yet finalized
ARROW_EXPORT
Result<Datum> Xor(const Datum& left, const Datum& right, ExecContext* ctx = NULLPTR);

/// For set lookup operations like IsIn, Match
struct ARROW_EXPORT SetLookupOptions : public FunctionOptions {
  explicit SetLookupOptions(Datum value_set, bool skip_nulls)
      : value_set(std::move(value_set)), skip_nulls(skip_nulls) {}

  Datum value_set;
  bool skip_nulls;
};

/// \brief IsIn returns true for each element of `values` that is contained in
/// `value_set`
///
/// If null occurs in left, if null count in right is not 0,
/// it returns true, else returns null.
///
/// \param[in] values array-like input to look up in value_set
/// \param[in] value_set either Array or ChunkedArray
/// \param[in] ctx the function execution context, optional
/// \return the resulting datum
///
/// \since 1.0.0
/// \note API not yet finalized
ARROW_EXPORT
Result<Datum> IsIn(const Datum& values, const Datum& value_set,
                   ExecContext* ctx = NULLPTR);

/// \brief Match examines each slot in the values against a value_set array.
/// If the value is not found in value_set, null will be output.
/// If found, the index of occurrence within value_set (ignoring duplicates)
/// will be output.
///
/// For example given values = [99, 42, 3, null] and
/// value_set = [3, 3, 99], the output will be = [1, null, 0, null]
///
/// Note: Null in the values is considered to match
/// a null in the value_set array. For example given
/// values = [99, 42, 3, null] and value_set = [3, 99, null],
/// the output will be = [1, null, 0, 2]
///
/// \param[in] values array-like input
/// \param[in] value_set either Array or ChunkedArray
/// \param[in] ctx the function execution context, optional
/// \return the resulting datum
///
/// \since 1.0.0
/// \note API not yet finalized
ARROW_EXPORT
Result<Datum> Match(const Datum& values, const Datum& value_set,
                    ExecContext* ctx = NULLPTR);

// ----------------------------------------------------------------------
// Temporal functions

struct ARROW_EXPORT StrptimeOptions : public FunctionOptions {
  explicit StrptimeOptions(std::string format, TimeUnit::type unit)
      : format(format), unit(unit) {}

  std::string format;
  TimeUnit::type unit;
};

}  // namespace compute
}  // namespace arrow
