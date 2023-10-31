//
// Created by hiccup on 2023/10/20.
//

#ifndef COMPILER_LLVM_IR_IRBUILDER_H
#define COMPILER_LLVM_IR_IRBUILDER_H

#include <vector>
#include <memory>
#include "Function.h"
#include "Module.h"
#include "constant/Constant.h"
#include "constant/ConstantInt.h"
#include "constant/ConstantArray.h"
#include "constant/ConstantString.h"
#include "instruction/AllocaInst.h"
#include "instruction/StoreInst.h"
#include "instruction/GEPInst.h"
#include "instruction/AluInst.h"
#include "instruction/ReturnInst.h"
#include "instruction/LoadInst.h"
#include "instruction/CallInst.h"

using namespace std;

struct IrCtx {
	bool isConst;
	bool isGlobal;
	bool isLeftValue;
	bool voidFunc;
	bool afterFuncDef;
	int loopNum = 0;
	int layerNum = 0;
};

struct IrRet {
	bool hasRet;
	int retLineNum;
	int rbraceLineNum;
	int dim;
	bool undefined = false;

	vector<int> dims{};
	//    ErrorRet()
};

using IrCtxPtr = std::shared_ptr<IrCtx>;
using IrRetPtr = std::shared_ptr<IrRet>;

static std::string globalPrefix = "@g_";
static std::string funcPrefix = "@f_";
static std::string localVarPrefix = "%t_";
static std::string bbPrefix = "b";
static std::string argPrefix = "%a_";

class IrBuilder {
	int bbCnt;
	int argsCnt;
	int globalVarCnt;
	int strLiteralCnt;
	int localVarCnt;

	Function *curFunc{};
	BasicBlock *curBb{};
	Module *module{};
public:
	IrCtx ctx = IrCtx();

	IrBuilder()
		: bbCnt(0), argsCnt(0), globalVarCnt(0), strLiteralCnt(0), localVarCnt(0) {}

	void addArgument(Argument *arg) {
		arg->setFunction(curFunc);
		curFunc->addArgument(arg);
	}

	void addFunction(Function *func) {
		module->addFunction(func);
	}

	void addBasicBlock(BasicBlock *bb) {
		curFunc->addBasicBlock(bb);
		bb->setParent(curFunc);
	}

	void addGlobalVariable(GlobalVariable *globalVar) {
		module->addGlobalVariable(globalVar);
	}

	std::string genFuncName(const std::string &name) {
		if (name == "main") {
			return "@main";
		}
		return funcPrefix + name;
	}

	std::string genBbName() {
		return bbPrefix + "_" + to_string(bbCnt++);
	}

	std::string genGlobalVarName(const std::string &name = "") {
		return globalPrefix + name + "_" + to_string((globalVarCnt++));
	}

	std::string genLocalVarName(const std::string &name = "") {
		return localVarPrefix + name + "_" + to_string((localVarCnt++));
//		return localVarPrefix + to_string((localVarCnt++));
	}

	std::string genArgName(const std::string &name = "") {
		return argPrefix + name + "_" + to_string((argsCnt++));
	}

	Function *getCurFunc() const { return curFunc; }

	void setCurFunc(Function *CurFunc) { curFunc = CurFunc; }

	BasicBlock *getCurBb() const { return curBb; }

	void setCurBb(BasicBlock *CurBb) { curBb = CurBb; }

	Module *getModule() const { return module; }

	void setModule(Module *Module) { module = Module; }

	Value *buildGlobalVar(const string &ident, Constant *constant, bool isConst) {
		string name = genGlobalVarName(ident);
		auto *globalVar = new GlobalVariable(new PointerType(constant->getType()), name, isConst, constant);
		module->addGlobalVariable(globalVar);
		return globalVar;
	}

	Value *buildArgument(const string &ident, Type *ty) {
		string name = genArgName(ident);
		auto *arg = new Argument(ty, name);
		curFunc->addArgument(arg);
		return arg;
	}

	Value *buildAlloc(const string &ident, Type *ty) {
		string name = genLocalVarName(ident);
		auto *alloca = new AllocaInst(ty, name);
		curBb->addInstruction(alloca);
		return alloca;
	}

	void buildStore(Value *content, Value *addr) {
		auto *store = new StoreInst(content, addr);
		curBb->addInstruction(store);
	}

	Value *buildLoad(Value *addr) {
		string name = genLocalVarName();
		auto *load = new LoadInst(name, addr);
		curBb->addInstruction(load);
		return load;
	}

	Value *buildGEP(Value *base, Value *ptrOff, Value *arrOff = nullptr) {
		string name = genLocalVarName();
		Instruction *gep;
		if (base->getType()->getTargetType()->isArray())
			gep = new GEPInst(name, base, new ConstantInt(0), ptrOff);
		else
			gep = new GEPInst(name, base, ptrOff, arrOff);

		curBb->addInstruction(gep);
		return gep;
	}

	/// contains @add, @sub, @sidv, @srem, @mul
	Value *buildAlu(AluType ty, Value *op1, Value *op2) {
		string name = genLocalVarName();
		auto *alu = new AluInst(name, ty, op1, op2);
		curBb->addInstruction(alu);
		return alu;
	}

	Value *buildReturn(Value *ret) {
		auto *retInst = new ReturnInst("returnInst", ret);
		curBb->addInstruction(retInst);
		return retInst;
	}

	Value *buildCall(Function *func, vector<Value *> args) {
		string name = genLocalVarName();
		auto *call = new CallInst(name, func, args);
		curBb->addInstruction(call);
		return call;
	}

};

extern IrBuilder irBuilder;

#endif //COMPILER_LLVM_IR_IRBUILDER_H
