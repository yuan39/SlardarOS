#ifndef __RING_H
#define __RING_H
#include "defs.h"
#include "init/initialization.h"
#include "utils/utils.h"

namespace os {
namespace intr {

using os::init::GDTSelector;

enum SystemCallType {
	nop = 0
};

void systemCall(SystemCallType systemCall = SystemCallType::nop);

template <typename T>
void enterRing3(const T& functor) {
	// TODO: Decide which %esp to use
	__asm__("movl %%esp, %%eax"
			:"=a"(os::init::tss.espRing0));
	__asm__("pushl %%eax;"
			::"a"(GDTSelector::dataUser + GDTSelector::RPL3));
	__asm__("pushl %%eax;"
			::"a"(TOP_OF_USER_STACK));
	__asm__("pushl %%eax;"
			::"a"(GDTSelector::codeUser + GDTSelector::RPL3));
	__asm__("pushl $Ring3;"
			"retf;");
	__asm__("Ring3:");
	os::utils::setSegmentRegisters(GDTSelector::dataUser + GDTSelector::RPL3);

	functor();

	systemCall();
	while (true) {}
}

void kernelProxy(SystemCallType);

} // intr
} // os

#endif