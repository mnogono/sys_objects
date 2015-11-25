//---------------------------------------------------------------------------
#ifndef sysControlStateH
#define sysControlStateH
//---------------------------------------------------------------------------

namespace sysControlState {
#ifdef __BORLANDC__
	//return control state as json object
	std::unique_ptr<TJSONObject> GetControlState(TControl *control);

	//restore control state from json settings
	void RestoreControlState(std::unique_ptr<TJSONObject> &jsonAppSettings, TControl *control, String prefix = NULL);
#endif
}

#endif
