#pragma once

#include "public.sdk/source/vst/vsteditcontroller.h"
#include "public.sdk/source/vst/vstparameters.h"
#include "pluginterfaces/vst/ivstmidicontrollers.h"
#include "pluginterfaces/base/ustring.h"

#include "buzzsawProcessor.h"

namespace exco {

using namespace Steinberg;
using namespace Vst;

class BuzzsawController : public EditControllerEx1
{
public:
	BuzzsawController();
	~BuzzsawController();
	
	tresult PLUGIN_API initialize(FUnknown* context) SMTG_OVERRIDE;

	ParameterContainer& getParameters() { return parameters; }

	DELEGATE_REFCOUNT(EditControllerEx1)

	static FUnknown* createInstance (void*) { return (IEditController*)new BuzzsawController; }
	static FUID uid;

};

} // namespace exco
