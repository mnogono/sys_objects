//---------------------------------------------------------------------------

#ifndef sysJSONH
#define sysJSONH
//---------------------------------------------------------------------------

namespace sysJSON {
#ifdef __BORLANDC__
	/* return NULL if can't find specify key */
	TJSONPair * GetPair(const TJSONObject *json, const wchar_t *key);

	/* return NULL if can't find specify key */
	TJSONValue * Get(const TJSONObject *json, const wchar_t *key);

	/*
	replace each \" in string value of JSON by \\\"
	replace like L"\\\""
	ancestor = NULL
	*/
	TJSONValue * ScreenQuotes(TJSONValue *json, const wchar_t *replace, TJSONAncestor *ancestor);

	/*
	replace like L"\\\\"
	ancestor = NULL
	*/
	TJSONValue * ScreenSlash(TJSONValue *json, const wchar_t *replace, TJSONAncestor *ancestor);

	/*
	ancestor = NULL
	*/
	TJSONValue * Screen(TJSONValue *json, const wchar_t *find, const wchar_t *replace, TJSONAncestor *ancestor);

	TJSONValue * Screen(TJSONValue *jsonValue);

	/*
	parse JSON file
	@param sFileName
	@param codePage [default CP_UTF8]
	@return JSONObject * || NULL

	don't forget to specify locale before using this function example: setlocale(LC_ALL, ".1251");
	*/
	TJSONObject * ParseJSONFile(const wchar_t *sFileName, unsigned int codePage = CP_UTF8);

	/*
	Check is jsonValue == null or ""
	*/
	bool IsEmpty(TJSONValue *jsonValue);
#endif
};

#endif
