#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

using namespace llvm;

namespace {

class LoopWrapperPass : public PassInfoMixin<LoopWrapperPass> {
   public:
    PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM) {
        errs() << "[DEBUG] Running on module: " << M.getName() << "\n";

        LLVMContext &Ctx = M.getContext();

        FunctionCallee loopStartFunc = M.getOrInsertFunction(
            "loop_start", FunctionType::get(Type::getVoidTy(Ctx), false));
        FunctionCallee loopEndFunc = M.getOrInsertFunction(
            "loop_end", FunctionType::get(Type::getVoidTy(Ctx), false));

        auto &FAM =
            MAM.getResult<FunctionAnalysisManagerModuleProxy>(M).getManager();

        int totalLoops = 0;
        for (Function &F : M) {
            if (F.isDeclaration()) {
                continue;
            }

            errs() << "[DEBUG] Processing function: " << F.getName() << "\n";

            auto &LI = FAM.getResult<LoopAnalysis>(F);
            int loopCount = 0;

            for (Loop *L : LI) {
                processLoop(L, loopStartFunc, loopEndFunc, loopCount);
            }

            errs() << "[DEBUG] Found " << loopCount << " loop(s) in function "
                   << F.getName() << "\n";
            totalLoops += loopCount;
        }

        if (totalLoops == 0) {
            return PreservedAnalyses::all();
        }
        return PreservedAnalyses::none();
    }

   private:
    void processLoop(Loop *L, FunctionCallee loopStartFunc,
                     FunctionCallee loopEndFunc, int &loopCount) {
        BasicBlock *preHeader = L->getLoopPreheader();

        IRBuilder<> builder(preHeader);
        builder.SetInsertPoint(preHeader->getTerminator());
        builder.CreateCall(loopStartFunc);

        SmallVector<BasicBlock *> exitBlocks;
        L->getExitBlocks(exitBlocks);
        for (BasicBlock *exitBlock : exitBlocks) {
            IRBuilder<> builderEnd(exitBlock, exitBlock->getFirstInsertionPt());
            builderEnd.CreateCall(loopEndFunc);
        }

        ++loopCount;

        for (Loop *SubLoop : L->getSubLoops()) {
            processLoop(SubLoop, loopStartFunc, loopEndFunc, loopCount);
        }
    }
};

}  // namespace

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
    return {LLVM_PLUGIN_API_VERSION, "LoopWrapperPass", LLVM_VERSION_STRING,
            [](PassBuilder &PB) {
                PB.registerPipelineParsingCallback(
                    [](StringRef Name, ModulePassManager &FPM,
                       ArrayRef<PassBuilder::PipelineElement>) {
                        if (Name == "loopWrapper") {
                            FPM.addPass(LoopWrapperPass());
                            return true;
                        }
                        return false;
                    });
            }};
}
