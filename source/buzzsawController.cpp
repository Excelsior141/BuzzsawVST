#include "buzzsawController.h"
#include "pluginterfaces/base/ibstream.h"
#include "base/source/fstreamer.h"

#include "utility/logger.h"

namespace exco {

using namespace Steinberg;
using namespace Vst;

FUID BuzzsawController::uid(0x099C8AF4, 0x8FD4C812, 0x5E6E0AA8, 0x40F796D3);

BuzzsawController::BuzzsawController() 
{

}

BuzzsawController::~BuzzsawController()
{
}

tresult PLUGIN_API BuzzsawController::initialize(FUnknown* context)
{
	tresult res = EditControllerEx1::initialize(context);
	if (res == kResultOk)
	{
		UnitInfo uinfo;
		uinfo.id = kRootUnitId;
		uinfo.parentUnitId = kNoParentUnitId;
		uinfo.programListId = kNoProgramListId;
		UString name(uinfo.name, 128);
		name.fromAscii("Root");
		addUnit(new Unit(uinfo));
	}

	return res;
}

} // namespace exco
