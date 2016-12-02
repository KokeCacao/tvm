/*!
 *  Copyright (c) 2016 by Contributors
 * \file expr.cc
 */
#include <tvm/base.h>
#include <tvm/expr.h>
#include <tvm/ir.h>
#include <ir/IRPrinter.h>
#include <memory>

namespace dmlc {
DMLC_REGISTRY_ENABLE(::tvm::NodeFactoryReg);
}  // namespace dmlc

namespace tvm {

Range::Range(Expr begin, Expr end)
    : Range(std::make_shared<Halide::IR::RangeNode>(begin, end - begin)) {
  // TODO(tqchen) add simplify to end - begin
}

Range Range::make_with_min_extent(Expr min, Expr extent) {
  return Range(std::make_shared<Halide::IR::RangeNode>(min, extent));
}

IterVar::IterVar(Range dom, std::string var_name, std::string thread_tag)
    : IterVar(IterVarNode::make(Var(var_name, Int(32)), dom, thread_tag)) {}

IterVar IterVarNode::make(Var var, Range dom, std::string thread_tag) {
  std::shared_ptr<IterVarNode> n = std::make_shared<IterVarNode>();
  n->var = var;
  n->dom = dom;
  n->thread_tag = thread_tag;
  return IterVar(n);
}

Expr sum(Expr source, Array<IterVar> rdom) {
  return ir::Reduce::make("Add", source, rdom);
}

Expr max(Expr source, Array<IterVar> rdom) {
  return ir::Reduce::make("Max", source, rdom);
}

Expr min(Expr source, Array<IterVar> rdom) {
  return ir::Reduce::make("Min", source, rdom);
}

std::ostream& operator<<(std::ostream& os, const NodeRef& n) {  // NOLINT(*)
  IRPrinter(os).print(n);
  return os;
}

TVM_STATIC_IR_FUNCTOR(IRPrinter, vtable)
.set_dispatch<IterVarNode>([](const IterVarNode *op, IRPrinter *p) {
    p->stream << "iter_var(";
    if (op->var->name_hint.length() != 0) {
      p->stream  << op->var->name_hint << ", ";
    }
    p->stream << op->dom;
    if (op->thread_tag.length() != 0) {
      p->stream << ", " << op->thread_tag;
    }
    p->stream << ")";
  });

TVM_STATIC_IR_FUNCTOR(IRPrinter, vtable)
.set_dispatch<Halide::IR::RangeNode>([](const Halide::IR::RangeNode *op, IRPrinter *p) {
    p->stream << "range(min=" << op->min << ", ext=" << op->extent << ')';
  });

TVM_REGISTER_NODE_TYPE(IterVarNode);


}  // namespace tvm
