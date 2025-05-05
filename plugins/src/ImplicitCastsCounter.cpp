#include <map>
#include <string>

#include "clang/AST/AST.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;

namespace {

class ImplicitCastVisitor : public RecursiveASTVisitor<ImplicitCastVisitor> {
   public:
    explicit ImplicitCastVisitor(ASTContext *Context) : Context(Context) {}

    bool VisitFunctionDecl(FunctionDecl *FD) {
        if (!FD->hasBody()) return true;

        currentFuncName = FD->getNameAsString();
        castCounts.clear();

        TraverseStmt(FD->getBody());

        if (!castCounts.empty()) {
            llvm::outs() << "Implicit casts in func \"" << currentFuncName
                         << "\":\n";
            for (const auto &entry : castCounts) {
                llvm::outs()
                    << "  " << entry.first << ": " << entry.second << "\n";
            }
        }

        return true;
    }

    bool VisitImplicitCastExpr(ImplicitCastExpr *ICE) {
        QualType srcType = ICE->getSubExpr()->getType();
        QualType dstType = ICE->getType();

        // filter casts to the same type (e.g. float -> float)
        if (srcType.getAsString() == dstType.getAsString()) {
            return true;
        }

        std::string cast =
            srcType.getAsString() + " -> " + dstType.getAsString();
        castCounts[cast]++;

        return true;
    }

   private:
    ASTContext *Context;
    std::string currentFuncName;
    std::map<std::string, int> castCounts;
};

class ImplicitCastConsumer : public ASTConsumer {
   public:
    explicit ImplicitCastConsumer(ASTContext *Context) : Visitor(Context) {}

    void HandleTranslationUnit(ASTContext &Context) override {
        Visitor.TraverseDecl(Context.getTranslationUnitDecl());
    }

   private:
    ImplicitCastVisitor Visitor;
};

class ImplicitCastAction : public PluginASTAction {
   protected:
    std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                   llvm::StringRef) override {
        return std::make_unique<ImplicitCastConsumer>(&CI.getASTContext());
    }

    bool ParseArgs(const CompilerInstance &CI,
                   const std::vector<std::string> &args) override {
        return true;
    }

    PluginASTAction::ActionType getActionType() override {
        return AddBeforeMainAction;
    }
};

}

static FrontendPluginRegistry::Add<ImplicitCastAction> X(
    "implicit-casts-counter", "Counts implicit casts in functions");
