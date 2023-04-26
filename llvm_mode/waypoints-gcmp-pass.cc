#include "fuzzfactory.hpp"

using namespace llvm;

enum cmp {
    GT,
    GE,
    LT,
    LE,
};

class GCmpFeedback : public fuzzfactory::DomainFeedback<GCmpFeedback> {

    GlobalVariable* ProgramLocationHash = NULL;
    
    /* Updates a global that tracks the last program location before a wrapcmp_* function is called */
    void updateProgramLocation(IRBuilder<>& irb) {
        irb.CreateStore(createProgramLocation(), ProgramLocationHash);
    }

    /* Construct __wrap_[n]eq<bw> functions on-demand */
    Function* getWrapICmpFunction(cmp sign, unsigned bw) {
        StringRef prefix = "__wrap_";
        StringRef sign_str;
        switch (sign) {
            case GT:
                sign_str = "gt";
                break;
            case GE:
                sign_str = "ge";
                break;
            case LT:
                sign_str = "lt";
                break;
            case LE:
                sign_str = "le";
                break;
        };
        std::string fname = (prefix + sign_str + Twine(bw)).str();
        return resolveFunction(fname, getIntTy(1), {getIntTy(bw), getIntTy(bw)});
    }

    /* Return a function __wrap__<name> on-demand */
    Function* getWrapper(Function* cmpFunction) {
        std::string fname = ("__wrap_" + cmpFunction->getName()).str(); // Name is prefix + callee name
        return resolveFunction(fname, cmpFunction->getFunctionType()); // Same type as underlying callee
    }

public:
    GCmpFeedback(Module& M) : fuzzfactory::DomainFeedback<GCmpFeedback>(M, "__afl_gcmp_dsf") { 
        // Create a reference to the global variable "__wrapgcmp_program_loc"
        ProgramLocationHash = new GlobalVariable(M, Int32Ty, false, GlobalValue::ExternalLinkage, 0, "__wrapgcmp_program_loc");
    }

    /* Handle icmp instruction */
    void visitICmpInst(ICmpInst& I) {
        // Sanity check
        if (I.getNumOperands() != 2) {
            errs () << "Warning: icmp found with " << I.getNumOperands() << " operands" << "\n";
            return;
        }

        // Ensure that operands of icmp are integers (not vectors)
        Type* operandType = I.getOperand(0)->getType();
        assert(operandType == I.getOperand(1)->getType());

        // Handle '==' and '!='
        cmp sign;
        if (I.getPredicate() == ICmpInst::Predicate::ICMP_UGT || I.getPredicate() == ICmpInst::Predicate::ICMP_SGT) {
            sign = GT;
        } else if (I.getPredicate() == ICmpInst::Predicate::ICMP_UGE || I.getPredicate() == ICmpInst::Predicate::ICMP_SGE) {
            sign = GE;
        } else if (I.getPredicate() == ICmpInst::Predicate::ICMP_ULT || I.getPredicate() == ICmpInst::Predicate::ICMP_SLT) {
            sign = LT;
        } else if (I.getPredicate() == ICmpInst::Predicate::ICMP_ULE || I.getPredicate() == ICmpInst::Predicate::ICMP_SLE) {
            sign = LE;
        } else {
            return; // Only instrument signed and unsigned greater/less than
        }
        
        IntegerType* intType = dyn_cast<IntegerType>(operandType);
        if (intType == NULL) {
            return; // Only instrument integral comparisons (not structs etc)
        }

        unsigned bw = intType->getBitWidth();
        if (bw != 8 && bw != 16 && bw != 32 && bw != 64) {
            return; // Only instrument 8, 16, 32 and 64 bit comparisons
        }  

        // Set IR irb to point to current instruction
        IRBuilder<> irb = insert_before(I);

        // Set program location hash to a statically generated random value
        updateProgramLocation(irb);

        // Find wrapcmp function
        Function* func = getWrapICmpFunction(sign, bw);
        
        // Wrap cmp
        auto wrap_cmp = irb.CreateCall(func, { I.getOperand(0), I.getOperand(1) });

        // Replace icmp with function call if we created one
        I.replaceAllUsesWith(wrap_cmp);
    }
    
    
};

FUZZFACTORY_REGISTER_DOMAIN(GCmpFeedback);
