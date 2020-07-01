// Goal : clautext -> cpp

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <vector>
#include <map>

// no fast lib!

#define ARRAY_QUEUE std::deque // chk?
#define VECTOR std::vector
//int log_result = 0; // why?

#include <wiz/ClauText.h>

#define INLINE inline

#define NONE 0



INLINE bool __expr___bool(const const wiz::DataType&  str)
{
	return str == "TRUE";
}


INLINE wiz::DataType __expr___add(const const wiz::DataType&  s1, const const wiz::DataType&  s2)
{
	wiz::DataType temp;
	temp.SetInt(s1.ToInt() + s2.ToInt());
	return temp; // change to int, double, ..
}

INLINE wiz::DataType __expr___divide(const const wiz::DataType&  s1, const const wiz::DataType&  s2)
{
	wiz::DataType temp;
	temp.SetInt(s1.ToInt() / s2.ToInt());
	return temp; // change to int, double, ..
}

INLINE wiz::DataType __expr___modular(const const wiz::DataType&  s1, const const wiz::DataType&  s2)
{
	wiz::DataType temp;
	temp.SetInt(s1.ToInt() % s2.ToInt());
	return temp; // change to int, double, ..
}

INLINE wiz::DataType  __expr___COMP_LT_EQ(const const wiz::DataType&  s1, const const wiz::DataType&  s2)
{
	auto x = wiz::load_data::Utility::Compare(s1.ToString(), s2.ToString());
	if (x == "== 0" || x == "< 0") {
		return "TRUE";
	}
	return "FALSE";
}
INLINE wiz::DataType __expr___COMP_GT_EQ(const const wiz::DataType&  s1, const const wiz::DataType&  s2)
{
	auto x = wiz::load_data::Utility::Compare(s1.ToString(), s2.ToString());
	if (x == "== 0" || x == "> 0") {
		return "TRUE";
	}
	return "FALSE";
}

INLINE wiz::DataType __expr___OR(const const wiz::DataType&  x, const const wiz::DataType&  y)
{
	if (x == "TRUE" || y == "TRUE") {
		return "TRUE";
	}
	return "FALSE";
}
INLINE wiz::DataType __expr___AND(const const wiz::DataType&  x, const const wiz::DataType&  y)
{
	if (x == "TRUE" && y == "TRUE")
	{
		return "TRUE";
	}
	return "FALSE";
}

INLINE wiz::DataType __expr___EQ(const const wiz::DataType&  x, const const wiz::DataType&  y)
{
	if (wiz::load_data::Utility::Compare(x.ToString(), y.ToString()) == "== 0") {
		return "TRUE";
	}
	else {
		return "FALSE";
	}
}
// find "/~"
INLINE wiz::DataType _Find(wiz::load_data::UserType& global, const const wiz::DataType&  str) {
	std::string x = wiz::load_data::LoadData::Find(&global, str.ToString());
	if (x.empty()) { return str; }
	return x;
}
INLINE wiz::DataType __expr___concat_all(const std::vector<wiz::DataType>& vec) // chk for -> no inline?
{
	wiz::DataType result;

	for (int i = 0; i < vec.size(); ++i) {
		result += vec[i];
	}
	return result;
}
INLINE wiz::DataType __expr___concat(const const wiz::DataType&  str1, const const wiz::DataType&  str2)
{
	return wiz::DataType(str1.ToString() + str2.ToString());
}

// todo - add wiz::ExecuteData parameter?
INLINE wiz::DataType __func___make(wiz::load_data::UserType& global, const wiz::ExecuteData& data, const const wiz::DataType&  str, const const wiz::DataType&  condition)
{
	std::string dir = str.ToString();

	dir = wiz::load_data::LoadData::ToBool4(nullptr, global, dir, data).ToString();

	std::string name;
	for (int i = dir.size() - 1; i >= 0; --i)
	{
		if (dir[i] == '/') {
			name = wiz::String::substring(dir, i + 1);
			dir = wiz::String::substring(dir, 0, i - 1);
			break;
		}
	}
	if (dir.empty()) { dir = "."; }

	wiz::load_data::LoadData::AddUserType(global, dir, name, "",  data);

	return "";
}
INLINE wiz::DataType __func___assign(wiz::load_data::UserType& global, const wiz::ExecuteData& executeData, wiz::ArrayMap<std::string, std::string>& locals, const const wiz::DataType& dir, const const wiz::DataType& value, const const wiz::DataType& condition)
{
	std::pair<std::string, std::string> _dir = wiz::load_data::LoadData::Find2(&global, dir.ToString());

	//// todo!! - add locals to parameter.
	if (_dir.first == "" && wiz::String::startsWith(_dir.second, "$local."))
	{
		locals[wiz::String::substring(_dir.second, 7)] = value.ToString();
	}
	else
	{
		wiz::load_data::LoadData::SetData(global, _dir.first, _dir.second, value.ToString(),  executeData);
	}
	return "";
}

INLINE wiz::DataType __func___assign2(wiz::load_data::UserType& global, const wiz::ExecuteData& executeData, wiz::ArrayMap<std::string, std::string>& locals,
	const const wiz::DataType& _dir, const const wiz::DataType& value, const const wiz::DataType& condition)
{
	std::pair<std::string, std::string> dir = wiz::load_data::LoadData::Find2(&global, _dir.ToString());

	{
		if (dir.first == "" && wiz::String::startsWith(dir.second, "$local."))
		{
			locals[wiz::String::substring(dir.second, 7)] = value.ToString();
		}
		else {
			wiz::load_data::LoadData::SetData(global, dir.first, dir.second, value.ToString(), executeData);
		}
	}

	return "";
}


INLINE wiz::DataType __func___insert(wiz::load_data::UserType& global, const wiz::ExecuteData& executeData, const const wiz::DataType& dir, const const wiz::DataType& value, const const wiz::DataType& condition)
{
	wiz::load_data::LoadData::AddData(global, dir.ToString(), value.ToString(),  executeData);

	return "";
}

template <class T>
INLINE wiz::DataType ToString(const T& data)
{
	return _toString(data);
}

template <>
INLINE wiz::DataType ToString(const const wiz::DataType& data)
{
	return data;
}


INLINE wiz::DataType CONCAT_ALL(const std::vector<wiz::DataType>& vec)
{
	wiz::DataType result;

	for (int i = 0; i < vec.size(); ++i) {
		result += vec[i];
		if (i < vec.size() - 1) {
			result += " ";
		}
	}

	return result;
}

INLINE wiz::DataType __expr___NOTEQ(const std::string x, const std::string y)
{
	return (wiz::load_data::Utility::Compare(x, y) == "== 0") ? "FALSE" : "TRUE";
}


INLINE wiz::DataType __expr___AND_ALL(const std::vector<wiz::DataType>& vec)
{
	bool result = true;

	for (int i = 0; i < vec.size(); ++i) {
		result = result && (vec[i] == "TRUE");
		if (vec[i] != "TRUE") {
			break;
		}
	}

	return result ? "TRUE" : "FALSE";
}

INLINE wiz::DataType __expr___COMP_GT_(const const wiz::DataType& x, const const wiz::DataType& y)
{
	return (wiz::load_data::Utility::Compare(x.ToString(), y.ToString()) == "> 0") ? "TRUE" : "FALSE";
}
INLINE wiz::DataType __expr___COMP_LT_(const const wiz::DataType& x, const const wiz::DataType& y)
{
	return (wiz::load_data::Utility::Compare(x.ToString(), y.ToString()) == "< 0") ? "TRUE" : "FALSE";
}


INLINE wiz::DataType __expr___is_quoted_str(const const wiz::DataType& x)
{
	std::string _x = x.ToString();

	if (_x.size() >= 2 && _x[0] == _x.back() && _x[0] == '\"') {
		return "TRUE";
	}
	return "FALSE";
}


#define __expr___return_value() result 

INLINE wiz::DataType __expr___element(wiz::load_data::UserType& global, const const wiz::DataType& dir, const const wiz::DataType& idx_str) {
	int idx = stoi(idx_str.ToString());

	if ('/' == dir.ToString()[0])
	{
		wiz::load_data::UserType* ut = wiz::load_data::UserType::Find(&global, dir.ToString()).second[0];
		return wiz::ToString(ut->GetItemList(idx).Get(0));
	}
	else
	{
		return "NONE";
	}
}


INLINE wiz::DataType __func___setElement(wiz::load_data::UserType& global, const wiz::ExecuteData& executeData, const const wiz::DataType& dir, const const wiz::DataType& idx, const const wiz::DataType& value)
{
	int _idx = stoi(idx.ToString());
	wiz::load_data::UserType::Find(&global, dir.ToString()).second[0]->SetItem(_idx, value);

	return "";
}

INLINE wiz::DataType __expr___size(wiz::load_data::UserType* global, const  wiz::DataType& dir)
{
	auto x = wiz::load_data::UserType::Find(global, dir.ToString());

	if (x.first) {
		return wiz::_toString(x.second[0]->GetItemListSize());
	}
	else {
		throw "error in __expr___size function.";
	}
}

//// todo - now.~ !
INLINE wiz::DataType __expr___get(wiz::load_data::UserType* global, const wiz::DataType& data, wiz::ArrayMap<std::string, std::string>& locals, wiz::ArrayMap<std::string, std::string>& parameters)
{
	std::string result;
	std::string _data = data.ToString();

	if (_data.size() > 1 && _data[0] == '@') {
		_data.erase(_data.begin());
	}

	// /./dir
	try {
		if (_data.size() > 1 && _data[0] == '/') {
			std::string x = wiz::load_data::LoadData::Find(global, _data);
			if (!x.empty()) {
				return x;
			}
		}
	}
	catch (...) {

	}
	// $local.
	try {
		std::string x = wiz::load_data::LoadData::FindLocals(locals, _data);
		if (!x.empty()) {
			return x;
		}
	}
	catch (...) {

	}
	// $parameter.
	try {
		std::string x = wiz::load_data::LoadData::FindParameters(parameters, _data);
		if (!x.empty()) {
			return x;
		}
	}
	catch (...) {

	}

	return "";
}

INLINE bool __expr___NOT(const bool x) {
	return !x;
}

INLINE wiz::DataType __expr___NOT(const const wiz::DataType& x) {
	return x == "TRUE" ? "FALSE" : "TRUE";
}



INLINE wiz::DataType __expr___rand(const const wiz::DataType& first, const const wiz::DataType& last) {
	int x = stoi(first.ToString());
	int y = stoi(last.ToString());
	int istart = std::min<int>(x, y), ilast = std::max<int>(x, y);

	return wiz::_toString(rand() % (ilast - istart + 1) + istart);
}

INLINE wiz::DataType __expr___pop_back(wiz::load_data::UserType* global, const const wiz::DataType& dir)
{
	std::string result;

	wiz::load_data::UserType* ut = nullptr;
	auto finded = wiz::load_data::UserType::Find(global, dir.ToString());
	ut = finded.second[0];

	if (ut->IsItemList(ut->GetIListSize() - 1))
	{
		auto x = ut->GetItemList(ut->GetItemListSize() - 1);
		result = wiz::ToString(x.Get(0));
		ut->RemoveItemList(ut->GetItemListSize() - 1);
	}
	else {
		auto x = ut->GetUserTypeList(ut->GetUserTypeListSize() - 1);
		result = (x->ToString()); // chk!!
		ut->RemoveUserTypeList(ut->GetUserTypeListSize() - 1);
	}

	return result;
}


INLINE wiz::DataType __func___swap(wiz::load_data::UserType* global, const wiz::ExecuteData& executeData,
	const const wiz::DataType& dir, const const wiz::DataType& value1, const const wiz::DataType& value2)
{
	std::string _dir = dir.ToString();

	if (value1 != value2) {
		int x = stoi(value1.ToString());
		int y = stoi(value2.ToString());

		std::string temp = wiz::ToString(wiz::load_data::UserType::Find(global, _dir).second[0]->GetItemList(x).Get(0));
		std::string temp2 = wiz::ToString(wiz::load_data::UserType::Find(global, _dir).second[0]->GetItemList(y).Get(0));

		wiz::load_data::LoadData::SetData(*global, _dir, x, temp2, executeData);
		wiz::load_data::LoadData::SetData(*global, _dir, y, temp, executeData);
	}
	return "";
}


INLINE wiz::DataType __func___findIndex(wiz::load_data::UserType* global, const wiz::ExecuteData& executeData, const const wiz::DataType& dir, const const wiz::DataType& value)
{
	wiz::load_data::UserType ut;
	wiz::load_data::LoadData::LoadDataFromString(wiz::load_data::UserType::Find(global, dir.ToString()).second[0]->ToString(), ut);

	for (int i = 0; i < ut.GetItemListSize(); ++i) {
		if (ut.GetItemList(i).Get(0) == value) {
			return wiz::toStr(i);
		}
	}

	return "";
}



INLINE wiz::DataType __func___remove3(wiz::load_data::UserType* global, const wiz::ExecuteData& executeData, const const wiz::DataType& dir, const const wiz::DataType& value, const const wiz::DataType& condition)
{
	int idx = stoi(value.ToString());  // long long -> int?

	wiz::load_data::LoadData::Remove(*global, dir.ToString(), idx,  executeData);

	return "";
}

INLINE wiz::DataType __func___save_data_only2(wiz::load_data::UserType* global, const wiz::ExecuteData& executeData, const const wiz::DataType& dir_name, const const wiz::DataType& file_name, const const wiz::DataType& option)
{
	auto dir = wiz::load_data::UserType::Find(global, dir_name.ToString());
	wiz::load_data::LoadData::SaveWizDB(*dir.second[0], file_name.ToString(), option.ToString());

	return "";
}

void Iterate(wiz::load_data::UserType* global, wiz::load_data::UserType* dirUT,
	const std::vector<std::pair<bool, wiz::DataType>(*)(wiz::load_data::UserType*, wiz::ExecuteData, wiz::ArrayMap<std::string, std::string>&)>& ids,
	const wiz::DataType& recursive, int depth, bool is_module, const wiz::DataType& module_name, const wiz::DataType& real_dir, wiz::ArrayMap<std::string, std::string>& parameters)
{

	for (int m = 0; m < dirUT->GetItemListSize(); ++m) {
		std::string result = parameters["before_value"];
		for (int k = 0; k < ids.size(); ++k) {
			wiz::ArrayMap<std::string, std::string> param;

			param.insert(std::make_pair<std::string, std::string>("name", wiz::ToString(dirUT->GetItemList(m).GetName())));
			param.insert(std::make_pair<std::string, std::string>("value", wiz::ToString(dirUT->GetItemList(m).Get(0))));
			param.insert(std::make_pair<std::string, std::string>("is_user_type", "FALSE"));
			param.insert(std::make_pair<std::string, std::string>("real_dir", real_dir.ToString()));
			auto temp = std::pair<std::string, std::string>("before_value", result);
			param.insert(temp);
			param.insert(std::make_pair<std::string, std::string>("idx", wiz::_toString(m)));

			auto x = ids[k](global, wiz::ExecuteData(), param);
			result = x.second.ToString();

			if (x.first) {
				wiz::load_data::UserType ut;
				wiz::load_data::LoadData::LoadDataFromString(x.second.ToString(), ut);

				if (!ut.GetItem("name").empty()) {
					dirUT->GetItemList(m).SetName(ut.GetItem("name")[0].Get(0));
				}
				if (!ut.GetItem("value").empty()) {
					dirUT->GetItemList(m).Set(0, ut.GetItem("value")[0].Get(0));
				}

				auto pass = ut.GetItem("pass");
				if (pass.empty() == false && pass[0].Get(0).ToString() == "TRUE") {
					break; // return;
				}
			}
		}
	}

	for (int m = 0; m < dirUT->GetUserTypeListSize(); ++m) {
		std::string result = parameters["before_value"];

		for (int k = 0; k < ids.size(); ++k) {
			wiz::ArrayMap<std::string, std::string> param;

			param.insert(std::make_pair<std::string, std::string>("name", wiz::ToString(dirUT->GetUserTypeList(m)->GetName())));
			param.insert(std::make_pair<std::string, std::string>("value", "NONE"));
			param.insert(std::make_pair<std::string, std::string>("is_user_type", "TRUE"));
			param.insert(std::make_pair<std::string, std::string>("real_dir", real_dir.ToString() + dirUT->GetUserTypeList(m)->GetName().ToString() + "/"));
			auto temp = std::pair<std::string, std::string>("before_value", result);
			param.insert(temp);

			param.insert(std::make_pair<std::string, std::string>("idx", wiz::_toString(m)));

			auto x = ids[k](global, wiz::ExecuteData(), param);
			result = x.second.ToString();

			if (x.first) {
				wiz::load_data::UserType ut;
				wiz::load_data::LoadData::LoadDataFromString(x.second.ToString(), ut);

				if (!ut.GetItem("name").empty()) {
					dirUT->GetUserTypeList(m)->SetName(ut.GetItem("name")[0].Get(0));
				}

				auto pass = ut.GetItem("pass");
				if (pass.empty() == false && pass[0].Get(0).ToString() == "TRUE") {
					break; // return;
				}
			}	
		}
		if ("TRUE" == recursive) {
			const std::string name = wiz::ToString(dirUT->GetName());
			Iterate(global, dirUT->GetUserTypeList(m), ids, recursive, depth, is_module, module_name,
				real_dir + "/" + (name.empty() ? "_" : name), parameters);
		}
	}
}


void Iterate2(wiz::load_data::UserType* global, wiz::load_data::UserType* dirUT,
	const std::vector<std::pair<bool, wiz::DataType>(*)(wiz::load_data::UserType*, wiz::ExecuteData, wiz::ArrayMap<std::string, std::string>&)>& ids,
	const wiz::DataType& recursive, int depth, bool is_module, const wiz::DataType& module_name, const wiz::DataType& real_dir, wiz::ArrayMap<std::string, std::string>& parameters)
{
	int itemCount = dirUT->GetItemListSize();
	int utCount = dirUT->GetUserTypeListSize();

	for (int m = dirUT->GetItemListSize() - 1; m >= 0; --m) {
		std::string result = parameters["before_value"];
		for (int k = 0; k < ids.size(); ++k) {
			itemCount--;
			wiz::ArrayMap<std::string, std::string> param;

			param.insert(std::make_pair<std::string, std::string>("name", wiz::ToString(dirUT->GetItemList(m).GetName())));
			param.insert(std::make_pair<std::string, std::string>("value", wiz::ToString(dirUT->GetItemList(m).Get(0))));
			param.insert(std::make_pair<std::string, std::string>("is_user_type", "FALSE"));
			param.insert(std::make_pair<std::string, std::string>("real_dir", real_dir.ToString()));
			auto temp = std::pair<std::string, std::string>("before_value", result);
			param.insert(temp);

			auto x = ids[k](global, wiz::ExecuteData(), param);
			result = x.second.ToString();

			if (x.first) {
				wiz::load_data::UserType ut;
				wiz::load_data::LoadData::LoadDataFromString(x.second.ToString(), ut);

				if (!ut.GetItem("name").empty()) {
					dirUT->GetItemList(m).SetName(ut.GetItem("name")[0].Get(0));
				}
				if (!ut.GetItem("value").empty()) {
					dirUT->GetItemList(m).Set(0, ut.GetItem("value")[0].Get(0));
			}
			}
		}
	}
	for (int m = dirUT->GetUserTypeListSize() - 1; m >= 0; --m) {
		std::string result = parameters["before_value"];
		for (int k = 0; k < ids.size(); ++k) {
			utCount--;
			wiz::ArrayMap<std::string, std::string> param;

			param.insert(std::make_pair<std::string, std::string>("name", wiz::ToString(dirUT->GetUserTypeList(m)->GetName())));
			param.insert(std::make_pair<std::string, std::string>("value", "NONE"));
			param.insert(std::make_pair<std::string, std::string>("is_user_type", "TRUE"));
			param.insert(std::make_pair<std::string, std::string>("real_dir", real_dir.ToString() + dirUT->GetUserTypeList(m)->GetName().ToString() + "/"));
			auto temp = std::pair<std::string, std::string>("before_value", result);
			param.insert(temp);


			auto x = ids[k](global, wiz::ExecuteData(), param);
			result = x.second.ToString();

			if (x.first) {
				wiz::load_data::UserType ut;
				wiz::load_data::LoadData::LoadDataFromString(x.second.ToString(), ut);

				if (!ut.GetItem("name").empty()) {
					dirUT->GetUserTypeList(m)->SetName(ut.GetItem("name")[0].Get(0));
				}
			}

			if ("TRUE" == recursive) {
				const std::string name = wiz::ToString(dirUT->GetName());
				Iterate2(global, dirUT->GetUserTypeList(m), ids, recursive, depth, is_module, module_name,
					real_dir + "/" + (name.empty() ? "_" : name), parameters);
			}
		}
	}
}

INLINE wiz::DataType __expr___remove_quoted(const const wiz::DataType& x)
{
	auto _x = x.ToString();

	if (_x.size() >= 2) {
		return _x.substr(1, _x.size() - 2);
	}
	else {
		return "";
	}
}


INLINE wiz::DataType __expr___move_up(std::string dir) {
	if (wiz::String::startsWith(dir, "/."))
	{
		dir = wiz::String::substring(dir, 3);
	}

	
	std::vector<std::string> tokenVec = wiz::tokenize(dir, '/');
	
	dir = "/./";
	if (tokenVec.empty()) { return dir; }
	for (int i = 0; i < tokenVec.size() - 1; ++i) {
		dir = dir + tokenVec[i] + "/";
	}
	return dir;
}

INLINE wiz::DataType __func___load_only_data2(wiz::load_data::UserType* global, const wiz::ExecuteData& executeData, wiz::DataType file_name, wiz::DataType dir_name, const wiz::DataType& lex_thr_num, const wiz::DataType& parse_thr_num)
{
	wiz::load_data::UserType* utTemp;

	std::string _file_name = file_name.ToString();

	if (_file_name.size() >= 2 && _file_name[0] == _file_name.back() && _file_name.back() == '\"') {
		file_name = wiz::DataType(_file_name.substr(1, _file_name.size() - 2));
	}

	if (dir_name == "/./" || dir_name == "root") {
		utTemp = global;
	}
	else {
		utTemp = global->GetUserTypeItem(dir_name.ToString())[0];
	}

	wiz::load_data::UserType ut;
	wiz::load_data::LoadData::LoadDataFromFile(file_name.ToString(), ut, std::stoi(parse_thr_num.ToString()) - 1, std::stoi(lex_thr_num.ToString()));

	{

		///for (int i = 0; i < ut.GetCommentListSize(); ++i) {
		//	utTemp->PushComment(std::move(ut.GetCommentList(i)));
		//}
		int item_count = 0;
		int userType_count = 0;

		for (int i = 0; i < ut.GetIListSize(); ++i) {
			if (ut.IsItemList(i)) {
				utTemp->AddItem(std::move(ut.GetItemList(item_count).GetName()),
					std::move(ut.GetItemList(item_count).Get(0)));
				item_count++;
			}
			else {
				utTemp->AddUserTypeItem(std::move(*ut.GetUserTypeList(userType_count)));
				userType_count++;
			}
		}

	}
	return "";
}

INLINE wiz::DataType __func___assign_from_ut(wiz::load_data::UserType& global, const wiz::ExecuteData& executeData, wiz::ArrayMap<std::string, std::string>& locals, const wiz::DataType& dir, const wiz::DataType& from, const wiz::DataType& value)
{
	std::pair<std::string, std::string> _dir = wiz::load_data::LoadData::Find2(&global, dir.ToString());

	//// todo!! - add locals to parameter.
	if (_dir.first == "" && wiz::String::startsWith(_dir.second, "$local."))
	{
		locals[wiz::String::substring(_dir.second, 7)] = value.ToString();
	}
	else
	{
		wiz::load_data::LoadData::SetData(global, _dir.first, _dir.second, value.ToString(), executeData);
	}
	return "";
}

INLINE wiz::DataType __expr___multiple(const wiz::DataType& x, const wiz::DataType& y) {
	return wiz::DataType(wiz::_toString(x.ToInt() * y.ToInt()));
}


INLINE wiz::DataType __expr___is_empty_string(const wiz::DataType& x) 
{
	return x.ToString().empty() ? "TRUE" : "FALSE";
}

INLINE wiz::DataType __expr___is_usertype_exist(const wiz::DataType& dir, wiz::load_data::UserType* global)
{
	return wiz::load_data::UserType::Find(global, dir.ToString()).first? "TRUE" : "FALSE";
}


INLINE wiz::DataType __expr___get_type(const wiz::DataType& type) {
	return wiz::load_data::Utility::GetType(type.ToString());
}
INLINE std::pair<bool, wiz::DataType> __event__list_sum_larger_than(wiz::load_data::UserType* global, wiz::ExecuteData& excuteData, wiz::ArrayMap<std::string, std::string>& parameters);
INLINE std::pair<bool, wiz::DataType> __event__larger_than(wiz::load_data::UserType* global, wiz::ExecuteData& excuteData, wiz::ArrayMap<std::string, std::string>& parameters);
INLINE std::pair<bool, wiz::DataType> __event__larger_than_300(wiz::load_data::UserType* global, wiz::ExecuteData& excuteData, wiz::ArrayMap<std::string, std::string>& parameters);
INLINE std::pair<bool, wiz::DataType> __event__int_string(wiz::load_data::UserType* global, wiz::ExecuteData& excuteData, wiz::ArrayMap<std::string, std::string>& parameters);
INLINE std::pair<bool, wiz::DataType> __event__float_string(wiz::load_data::UserType* global, wiz::ExecuteData& excuteData, wiz::ArrayMap<std::string, std::string>& parameters);
INLINE std::pair<bool, wiz::DataType> __event__int(wiz::load_data::UserType* global, wiz::ExecuteData& excuteData, wiz::ArrayMap<std::string, std::string>& parameters);
INLINE std::pair<bool, wiz::DataType> __event__false(wiz::load_data::UserType* global, wiz::ExecuteData& excuteData, wiz::ArrayMap<std::string, std::string>& parameters);

int main(void)
{
	wiz::ExecuteData excuteData;
	wiz::load_data::UserType global;
	srand(time(NULL));
	std::string fileName = "b.txt_test.txt";
	wiz::load_data::LoadData::LoadDataFromFile(fileName, global);

	wiz::load_data::UserType events;
	auto x = global.GetUserTypeItem("Event");
	for (int i = 0; i < x.size(); ++i) {
	events.AddUserTypeItem(*x[i]);
	} 	excuteData.chkInfo = true;	excuteData.pEvents = &events;
	wiz::ArrayMap<std::string, std::string> parameters;
	std::cout << __event__list_sum_larger_than(&global, excuteData, parameters).second.ToString() << std::endl;
	return 0;
}

INLINE std::pair<bool, wiz::DataType> __event__list_sum_larger_than(wiz::load_data::UserType* global, wiz::ExecuteData& excuteData, wiz::ArrayMap<std::string, std::string>& parameters)
{
	wiz::ArrayMap<std::string, std::string> locals;
	wiz::DataType result;
	bool result_change = false;
	std::string option;

	locals["sum"] = "";
	locals["i"] = "";

	__func___assign(*global, excuteData, locals, "$local.sum", CONCAT_ALL(std::vector<wiz::DataType>{ std::string() + "0"}), "TRUE");
	__func___assign(*global, excuteData, locals, "$local.i", CONCAT_ALL(std::vector<wiz::DataType>{ std::string() + "0"}), "TRUE");
	while (__expr___bool(__expr___COMP_LT_(locals["i"] , __expr___size(global, parameters["real_dir"]) ) )) {
		__func___assign(*global, excuteData, locals, "$local.sum", CONCAT_ALL(std::vector<wiz::DataType>{ std::string() + __expr___add(locals["sum"] , __expr___get(global, __expr___concat_all(std::vector<wiz::DataType>{ parameters["real_dir"] , "/" , "$" , "it" , locals["i"]} ) , locals, parameters ) ) }), "TRUE");
		__func___assign(*global, excuteData, locals, "$local.i", CONCAT_ALL(std::vector<wiz::DataType>{ std::string() + __expr___add(locals["i"] , "1") }), "TRUE");

	}
	if (__expr___bool(__expr___COMP_GT_(locals["sum"] , parameters["n"]) )) {
		result_change = true;
		return { result_change, CONCAT_ALL(std::vector<wiz::DataType>{ std::string() +  "TRUE"}) };

	}
	if (CONCAT_ALL(std::vector<wiz::DataType>{ std::string() + parameters["real_dir"]}).empty()) {
std::cout << "EMPTY";
}
else {
std::cout << CONCAT_ALL(std::vector<wiz::DataType>{ std::string() + parameters["real_dir"]});
}

	result_change = true;
	return { result_change, CONCAT_ALL(std::vector<wiz::DataType>{ std::string() +  "FALSE"}) };

	return { result_change, result };
}
INLINE std::pair<bool, wiz::DataType> __event__larger_than(wiz::load_data::UserType* global, wiz::ExecuteData& excuteData, wiz::ArrayMap<std::string, std::string>& parameters)
{
	wiz::ArrayMap<std::string, std::string> locals;
	wiz::DataType result;
	bool result_change = false;
	std::string option;


	if (__expr___bool(__expr___COMP_GT_(parameters["value"] , parameters["n"]) )) {
		result_change = true;
		return { result_change, CONCAT_ALL(std::vector<wiz::DataType>{ std::string() +  "TRUE"}) };

	}
	result_change = true;
	return { result_change, CONCAT_ALL(std::vector<wiz::DataType>{ std::string() +  "FALSE"}) };

	return { result_change, result };
}
INLINE std::pair<bool, wiz::DataType> __event__larger_than_300(wiz::load_data::UserType* global, wiz::ExecuteData& excuteData, wiz::ArrayMap<std::string, std::string>& parameters)
{
	wiz::ArrayMap<std::string, std::string> locals;
	wiz::DataType result;
	bool result_change = false;
	std::string option;


	if (__expr___bool(__expr___COMP_GT_(parameters["value"] , "300") )) {
		result_change = true;
		return { result_change, CONCAT_ALL(std::vector<wiz::DataType>{ std::string() +  "TRUE"}) };

	}
	result_change = true;
	return { result_change, CONCAT_ALL(std::vector<wiz::DataType>{ std::string() +  "FALSE"}) };

	return { result_change, result };
}
INLINE std::pair<bool, wiz::DataType> __event__int_string(wiz::load_data::UserType* global, wiz::ExecuteData& excuteData, wiz::ArrayMap<std::string, std::string>& parameters)
{
	wiz::ArrayMap<std::string, std::string> locals;
	wiz::DataType result;
	bool result_change = false;
	std::string option;


	if (__expr___bool(__expr___EQ("INTEGER" , __expr___get_type(__expr___remove_quoted(parameters["input"]) ) ) )) {
		result_change = true;
		return { result_change, CONCAT_ALL(std::vector<wiz::DataType>{ std::string() +  "TRUE"}) };

	}
	result_change = true;
	return { result_change, CONCAT_ALL(std::vector<wiz::DataType>{ std::string() +  "FALSE"}) };

	return { result_change, result };
}
INLINE std::pair<bool, wiz::DataType> __event__float_string(wiz::load_data::UserType* global, wiz::ExecuteData& excuteData, wiz::ArrayMap<std::string, std::string>& parameters)
{
	wiz::ArrayMap<std::string, std::string> locals;
	wiz::DataType result;
	bool result_change = false;
	std::string option;


	if (__expr___bool(__expr___EQ("FLOAT" , __expr___get_type(__expr___remove_quoted(parameters["input"]) ) ) )) {
		result_change = true;
		return { result_change, CONCAT_ALL(std::vector<wiz::DataType>{ std::string() +  "TRUE"}) };

	}
	result_change = true;
	return { result_change, CONCAT_ALL(std::vector<wiz::DataType>{ std::string() +  "FALSE"}) };

	return { result_change, result };
}
INLINE std::pair<bool, wiz::DataType> __event__int(wiz::load_data::UserType* global, wiz::ExecuteData& excuteData, wiz::ArrayMap<std::string, std::string>& parameters)
{
	wiz::ArrayMap<std::string, std::string> locals;
	wiz::DataType result;
	bool result_change = false;
	std::string option;


	if (__expr___bool(__expr___EQ("INTEGER" , __expr___get_type(parameters["input"]) ) )) {
		result_change = true;
		return { result_change, CONCAT_ALL(std::vector<wiz::DataType>{ std::string() +  "TRUE"}) };

	}
	result_change = true;
	return { result_change, CONCAT_ALL(std::vector<wiz::DataType>{ std::string() +  "FALSE"}) };

	return { result_change, result };
}
INLINE std::pair<bool, wiz::DataType> __event__false(wiz::load_data::UserType* global, wiz::ExecuteData& excuteData, wiz::ArrayMap<std::string, std::string>& parameters)
{
	wiz::ArrayMap<std::string, std::string> locals;
	wiz::DataType result;
	bool result_change = false;
	std::string option;


	result_change = true;
	return { result_change, CONCAT_ALL(std::vector<wiz::DataType>{ std::string() +  "FALSE"}) };

	return { result_change, result };
}
