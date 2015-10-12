//---------------------------------------------------------------------------

#include "pch.h"
#pragma hdrstop

#include "sysStr.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

#ifdef __BORLANDC__

namespace sysJSON {
	TJSONPair * GetPair(const TJSONObject *json, const wchar_t *key) {
		std::vector<std::wstring> keys;
		sysStr::Split(key, L".", keys);

		TJSONPair *pair;

		try {
			for (int itKey = 0, len = keys.size(); itKey < len; ++itKey, json = (TJSONObject *)pair->JsonValue) {
				pair = const_cast<TJSONObject *>(json)->Get(keys[itKey].c_str());
				if (!pair) return NULL;
			}
			return pair;

		} catch (...) {
			return NULL;
		}
	}


	TJSONValue * Get(const TJSONObject *json, const wchar_t *key) {
		TJSONPair *pair = GetPair(json, key);
		if (!pair) return NULL;

		return pair->JsonValue;
	}

	/*
class TMyJSONString : public TJSONString {
	public:
		void SetValue(String value)
};
*/

	TJSONValue * Screen(TJSONValue *jsonValue, const wchar_t *find, const wchar_t *replace, TJSONAncestor *ancestor) {
		if (dynamic_cast<TJSONArray *>(jsonValue)) {
			TJSONArray *jsonArray = (TJSONArray *)jsonValue;
			#if __CODEGEARC__  < 0x690
			for (int itKey = 0; itKey < jsonArray->Size(); ++itKey) {
				Screen(jsonArray->Get(itKey), find, replace, NULL);
			}
			#else
			for (int itKey = 0; itKey < jsonArray->Count; ++itKey) {
				sysJSON::Screen(jsonArray->Items[itKey], find, replace, NULL);
			}
			#endif

		} else if (dynamic_cast<TJSONObject *>(jsonValue)) {
			TJSONObject *jsonObject = (TJSONObject *)jsonValue;
			#if __CODEGEARC__  < 0x690
			for (int itKey = 0; itKey < jsonObject->Size(); ++itKey) {
				TJSONPair *jsonPair = jsonObject->Get(itKey);
				Screen(jsonPair->JsonValue, find, replace, jsonPair);
			}
			#else
			for (int itKey = 0; itKey < jsonObject->Count; ++itKey) {
				TJSONPair *jsonPair = jsonObject->Pairs[itKey];
				Screen(jsonPair->JsonValue, find, replace, jsonPair);
			}
			#endif

		} else if (dynamic_cast<TJSONString *>(jsonValue)) {
			TJSONString *jsonString = (TJSONString *)jsonValue;
			String value = jsonString->Value();
			if (!value.Pos(find)) return jsonValue;

			value = StringReplace(value, find, replace, TReplaceFlags() << rfReplaceAll);
			if (dynamic_cast<TJSONPair *>(ancestor)) {
				TJSONPair *jsonPair = (TJSONPair *)ancestor;
				jsonPair->JsonValue = new TJSONString(value);
			}
		}

		return jsonValue;
	}

	TJSONValue * ScreenSlash(TJSONValue *jsonValue, const wchar_t *replace, TJSONAncestor *ancestor){
		return Screen(jsonValue, L"\\", replace, ancestor);
	}

	TJSONValue * ScreenQuotes(TJSONValue *jsonValue, const wchar_t *replace, TJSONAncestor *ancestor){
		return sysJSON::Screen(jsonValue, L"\"", replace, ancestor);
	}

	TJSONValue * Screen(TJSONValue *jsonValue) {
		jsonValue = ScreenSlash(jsonValue, L"\\\\", NULL);
		jsonValue = ScreenQuotes(jsonValue, L"\\\"", NULL);

		return jsonValue;
	}

	TJSONObject * ParseJSONFile(const wchar_t *sFileName, unsigned int codePage) {
		struct _stat st;
		_wstat(sFileName, &st);
		if (!st.st_size) return NULL;

		char *buffer = new char[st.st_size + 1];
		/*
		std::ifstream in(sFileName, std::ios_base::binary);
		if (!in.good()) return NULL;

		in.read(buffer, st.st_size);
		int readedBytes = in.gcount();
		buffer[readedBytes] = 0;
		in.close();
		*/
		//refuse from std::ifstream because of CodeGuard
		FILE *f = _wfopen(sFileName, L"rb");
		if (!f) {
			return NULL;
		}

		int readedBytes = fread(buffer, sizeof(char), st.st_size, f);
		buffer[readedBytes] = 0;

		fclose(f);

		TJSONObject *jsonData = NULL;

		if (codePage == CP_UTF8) {
			int wbufferLen;
			wchar_t *wbuffer;
			sysStr::UTF8ToWideChar(buffer, readedBytes, wbuffer, wbufferLen);
			String jsonString(wbuffer);
			delete []wbuffer;
			jsonData = (TJSONObject *)TJSONObject::ParseJSONValue(jsonString);

			//delete []buffer;
			//delete []wbuffer;
		} else {
			String jsonString(buffer);
			jsonData = (TJSONObject *)TJSONObject::ParseJSONValue(jsonString);
			//delete []buffer;
		}

		delete []buffer;

		return jsonData;
	}

	bool IsEmpty(TJSONValue *jsonValue) {
		if (!jsonValue) return true;
		return jsonValue->Value().IsEmpty();
	}
};

#endif
