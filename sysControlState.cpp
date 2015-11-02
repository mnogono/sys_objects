//---------------------------------------------------------------------------
#include "pch.h"
#pragma hdrstop

#include "sysControlState.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

namespace sysControlState {

	std::unique_ptr<TJSONObject> GetLabelState(TLabel *label) {
		std::unique_ptr<TJSONObject> jsonState(new TJSONObject);
		jsonState->AddPair("Caption", label->Caption);

		return jsonState;
	}

	std::unique_ptr<TJSONObject> GetEditState(TEdit *edit) {
		std::unique_ptr<TJSONObject> jsonState(new TJSONObject);
		jsonState->AddPair("Text", edit->Text);

		return jsonState;
	}

	std::unique_ptr<TJSONObject> GetCheckBoxState(TCheckBox *checkBox) {
		std::unique_ptr<TJSONObject> jsonState(new TJSONObject);
		if (checkBox->Checked == true) {
			jsonState->AddPair("Checked", new TJSONTrue);
		} else {
			jsonState->AddPair("Checked", new TJSONFalse);
		}

		return jsonState;
	}

	std::unique_ptr<TJSONObject> GetDateTimeState(const TDateTime *dateTime) {
		std::unique_ptr<TJSONObject> jsonState(new TJSONObject);
		jsonState->AddPair("Val", dateTime->Val);

		return jsonState;
	}

	std::unique_ptr<TJSONObject> GetDateTimePickerState(TDateTimePicker *dateTimePicker) {
		std::unique_ptr<TJSONObject> jsonDateTimeState = GetDateTimeState(&(dateTimePicker->DateTime));

		std::unique_ptr<TJSONObject> jsonState(new TJSONObject);
		jsonState->AddPair("DateTime", jsonDateTimeState.release());

		return jsonState;
	}

	std::unique_ptr<TJSONObject> GetControlState(TControl *control) {
		if (dynamic_cast<TLabel *>(control)) {
			return GetLabelState(static_cast<TLabel *>(control));

		} else if (dynamic_cast<TEdit *>(control)) {
			return GetEditState(static_cast<TEdit *>(control));

		} else if (dynamic_cast<TCheckBox *>(control)) {
			return GetCheckBoxState(static_cast<TCheckBox *>(control));

		} else if (dynamic_cast<TDateTimePicker *>(control)) {
			return GetDateTimePickerState(static_cast<TDateTimePicker *>(control));
		}

		return std::unique_ptr<TJSONObject> (NULL);
	}

	void RestoreEditState(TJSONObject *jsonState, TEdit *edit) {
		edit->Text = jsonState->Values["Text"]->Value();
	}

	void RestoreCheckBoxState(TJSONObject *jsonState, TCheckBox *checkBox) {
		TJSONValue *jsonChecked = jsonState->Values["Checked"];
		checkBox->Checked = dynamic_cast<TJSONTrue *>(jsonChecked);
	}

	void RestoreDateTimeState(TJSONObject *jsonState, TDateTimePicker *dateTimePicker) {
		TJSONNumber *jsonVal = static_cast<TJSONNumber *>(jsonState->Values["Val"]);
		dateTimePicker->DateTime = jsonVal->AsDouble;
	}

	void RestoreDateTimePicker(TJSONObject *jsonState, TDateTimePicker *dateTimePicker) {
		if (jsonState->Values["DateTime"]) {
			RestoreDateTimeState(static_cast<TJSONObject *>(jsonState->Values["DateTime"]), dateTimePicker);
		}
	}

	void RestoreControlState(std::unique_ptr<TJSONObject> &jsonAppSettings, TControl *control, String prefix) {
		String key;
		if (prefix == NULL) {
			key = control->Name;
		} else {
			key = prefix + control->Name;
		}

		if (!jsonAppSettings->Values[key]) {
			return;
		}

		TJSONObject *jsonSettings = static_cast<TJSONObject *>(jsonAppSettings->Values[key]);
		if (dynamic_cast<TEdit *>(control)) {
			RestoreEditState(jsonSettings, static_cast<TEdit *>(control));

		} else if (dynamic_cast<TCheckBox *>(control)) {
			RestoreCheckBoxState(jsonSettings, static_cast<TCheckBox *>(control));

		} else if (dynamic_cast<TDateTimePicker *>(control)) {
        	RestoreDateTimePicker(jsonSettings, static_cast<TDateTimePicker *>(control));

		}
	}
};
