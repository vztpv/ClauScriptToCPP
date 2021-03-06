

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

// no fast lib!

#define ARRAY_QUEUE std::deque // chk?
#define VECTOR std::vector
int log_result = 0; // why?

#include <wiz/ClauText.h>

// " -> \" //
// ' -> \'
inline std::string ToStr(const std::string& str)
{
	std::string result;

	wiz::load_data::Utility::ChangeStr2(str, std::vector<std::string>{"\"", "\'", "\\"}, std::vector<std::string>{"\\\"", "\\\'", "\\\\"}, result);

	return result;
}

// need to rename!
void Do(const wiz::load_data::UserType& someUT, wiz::load_data::UserType& result)
{
	int it_count = 0; // int -> size_t?
	int ut_count = 0;

	for (int i = 0; i < someUT.GetIListSize(); ++i) {
		if (someUT.IsItemList(i)) {
			// chk case $concat_all = { x = 3 } => error?
			const std::string name = wiz::ToString(someUT.GetItemList(it_count).GetName());
			std::string data = wiz::ToString(someUT.GetItemList(it_count).Get(0));


			if (wiz::String::startsWith(data, "$local.")) {
				result.AddItem(ToStr(name), std::string("") + "locals[\"" + ToStr(data.substr(7)) + "\"]");
			//	result.PushComment("1");
			}
			else if (wiz::String::startsWith(data, "$parameter.")) {
				result.AddItem(ToStr(name), std::string("") + "parameters[\"" + ToStr(data.substr(11)) + "\"]");
			//	result.PushComment("1");
			}
			else if (wiz::String::startsWith(data, "/") && data.size() > 1) {
				//// todo - chk /./ABC/$parameter.j - do not allow?
				// concat_all("/", ".", "/", "ABC", "/", parameters["j"]) // chk exist?
				result.AddItem(ToStr(name), std::string("") + "_Find(*global, \"" + ToStr(data) + "\")");
			//	result.PushComment("1");
			}
			else {
				result.AddItem(ToStr(wiz::ToString(someUT.GetItemList(it_count).GetName())), ToStr(data));
			//	result.PushComment("2");
			}
			it_count++;
		}
		else { // UserType
			std::string name = wiz::ToString(someUT.GetUserTypeList(ut_count)->GetName());

			result.AddUserTypeItem(wiz::load_data::UserType(ToStr(name)));
			//result.PushComment("3");

			Do(*someUT.GetUserTypeList(ut_count), *result.GetUserTypeList(ut_count));
			ut_count++;
		}
	}

}

std::string PrintSomeUT(wiz::load_data::UserType& someUT, bool expr = false, int depth = 0, int option = 0) // option : return option 1
{
	std::string result;

	int it_count = 0;
	int ut_count = 0;
	for (int i = 0; i < someUT.GetIListSize(); ++i) {
		if (someUT.IsItemList(i)) {
			// chk case $concat_all = { x = 3 } => error?
			std::string data = wiz::ToString(someUT.GetItemList(it_count).Get(0));
			std::string name = wiz::ToString(someUT.GetItemList(it_count).GetName());
			
			int state = 0;
			if (wiz::String::startsWith(data, "locals[")) {
				state = 1;
			}
			else if (wiz::String::startsWith(data, "parameters[")) {
				state = 1;
			}
			else if (wiz::String::startsWith(data, "_Find(") && data.size() > 1) {
				state = 1;
			}
			else {
				state = 2;
			}

			if (name.empty()) {
				if (state != 2) {
					result += data;
				}
				else {
					result += "\"" + (data)+ "\"";
				}
			}
			else {
				if (state != 2) {
					result += "\"" + name + " = \", " + data;
				}
				else {
					result += "\"" + (name)+" = " + (data)+"\"";
				}
			}

			if (i < someUT.GetIListSize() - 1) {
				result += " , ";
			}


			it_count++;
		}
		else {
			//// todo - $ removal!!
			bool string_all = false;
			bool bool_all = false;
			bool return_value = false;
			bool is_function = false;
			bool no_name = wiz::ToString(someUT.GetUserTypeList(ut_count)->GetName()).empty();

			//// check..
			if (no_name) {
				auto x = PrintSomeUT(*someUT.GetUserTypeList(ut_count), false, depth + 1, 0);
				result += x;
				continue;
			}

			std::string x = wiz::ToString(someUT.GetUserTypeList(ut_count)->GetName());
			if ("$concat_all" == x) {
				string_all = true;
			}
			if ("$AND_ALL" == x) {
				bool_all = true;
			}
			if ("$return_value" == x) {
				if (expr) {
					return_value = true; // for $while, $if
				}
			}

			if (wiz::String::startsWith(x, "$") && x.size() > 1) { //cf) $."regex" ?
				x.erase(x.begin());
				is_function = true;

				x = wiz::String::replace(x, "<", "_LT_");
				x = wiz::String::replace(x, ">", "_GT_");
			}

			if (return_value) {
				//result += "(\"TRUE\" == ";
			}

			if (is_function && x == "event_result") {
				wiz::load_data::UserType* _func = someUT.GetUserTypeList(ut_count);
				std::string id;
				wiz::ArrayMap<std::string, std::string> param;

				{
					wiz::load_data::UserType* func = _func;
					for (int j = 0; j < func->GetItemListSize(); ++j) {
						if ("id" == wiz::ToString(func->GetItemList(j).GetName())) {
							id = ToStr(wiz::ToString(func->GetItemList(j).Get(0)));
						}
						else {
							std::string value = wiz::ToString(func->GetItemList(j).Get(0));
							wiz::load_data::UserType _ut;
							wiz::load_data::UserType resultUT;

							wiz::load_data::LoadData::LoadDataFromString(value, _ut);
							// for item, item in ut->GetUserTypeList(0), ... 
							Do(_ut, resultUT);

							value = "CONCAT_ALL(std::vector<wiz::DataType>{ std::string() + " + PrintSomeUT(resultUT) + "})";

							param[wiz::ToString(func->GetItemList(j).GetName())] = value;
						}
					}
					for (int j = 0; j < func->GetUserTypeListSize(); ++j) {
						if ("id" == func->GetUserTypeList(j)->GetName()) {
							wiz::load_data::UserType* ut = func->GetUserTypeList(j);
							wiz::load_data::UserType resultUT;
							// for item, item in ut->GetUserTypeList(0), ... 
							Do(*ut, resultUT);

							id = PrintSomeUT(resultUT);
						}
						else {
							wiz::load_data::UserType* ut = func->GetUserTypeList(j);
							wiz::load_data::UserType resultUT;
							// for item, item in ut->GetUserTypeList(0), ... 
							Do(*ut, resultUT);

							param[wiz::ToString(ut->GetName())] = PrintSomeUT(resultUT);
						}
					}
				}

				for (int i = 0; i < depth + 1; ++i) {
					result += "\t";
				}
				result += "(([&]() -> std::string { ";

				for (int i = 0; i < depth + 1; ++i) {
					result += "\t";
				}
				result += "wiz::ArrayMap<std::string, std::string> param;\n";
				for (const auto& x : param) {
					for (int i = 0; i < depth + 1; ++i) {
						result += "\t";
					}
					result += "param[\"" + x.first + "\"] = " + x.second + ";\n";
				}

				result += "\n";
				for (int i = 0; i < depth + 1; ++i) {
					result += "\t";
				}
				result += "auto x = __event__";
				result += id;
				result += "(global, excuteData, param);";
				result += "\n";

				for (int i = 0; i < depth + 1; ++i) {
					result += "\t";
				}

				result += "if (x.first) { return x.second; } else { return \"\"; } ";

				for (int i = 0; i < depth; ++i) {
					result += "\t";
				}

				result += "} ) ()";
			}
			else if (is_function) {
				result += "__expr___" + x + "(";
			}
			else {
				result += "\"" + x + " =  { \" + CONCAT_ALL(std::vector<wiz::DataType>{ std::string() + "; // chk!!
			}

			if (is_function && "element" == x) {
				result += "*global, ";
			}
			if (is_function && ("size" == x || "get" == x || "pop_back" == x)) {
				result += "global, ";
			}
			if (string_all) {
				result += "std::vector<wiz::DataType>{ ";
			}
			if (bool_all) {
				result += "std::vector<wiz::DataType>{ ";
			}

			if (is_function && x == "event_result") {

			}
			else {
				if (option == 1 && wiz::ToString(someUT.GetUserTypeList(ut_count)->GetName())[0] != '$') {
					result += " \"{\", ";
				}
				auto x = PrintSomeUT(*someUT.GetUserTypeList(ut_count), false, depth + 1, 0);
				result += x;

				if (option == 1 && wiz::ToString(someUT.GetUserTypeList(ut_count)->GetName())[0] != '$') {
					if (false == x.empty()) {
						result += " , ";
					}
					result += " \"} \"";
				}
			}


			if (string_all || bool_all) {
				result += "} ";
			}
			if (!is_function) {
				result += "}) + \" } \"";
			}
			if (return_value) {
				//result += ")";
			}

			if (is_function && "get" == x) {
				result += ", locals, parameters ";
			}
			if (is_function && "is_usertype_exist" == x) {
				result += ", global";
			}

			if (is_function) {
				result += ") ";
			}

			if (i < someUT.GetIListSize() - 1) {
				result += " , ";
			}

			ut_count++;
		}
	}

	if (result.empty()) {
		return "\"\"";
	}
	return result;
}



std::string ConvertFunction(wiz::load_data::UserType* global, wiz::load_data::UserType& _eventUT, wiz::load_data::UserType& eventUT, int depth = 1, const std::string& _option = "", bool is_module = false, const std::string& module_name = "") // , int depth,  for \t!
{
	std::string result;
	std::string option = _option;

	for (int i = 0; i < eventUT.GetUserTypeListSize(); ++i) {
		const std::string functionName = wiz::ToString(eventUT.GetUserTypeList(i)->GetName());
		if (functionName == "$local" ||
			functionName == "$parameter")
		{
			continue;
		}

		for (int i = 0; i < depth; ++i) {
			result += "\t";
		}

		//// check $push_back?, 
		if (functionName == "$assign_from_ut") {
			std::string from, value, as;

			{
				wiz::load_data::UserType* _ut = eventUT.GetUserTypeList(i)->GetUserTypeList(0);
				wiz::load_data::UserType _resultUT;
				// for item, item in ut->GetUserTypeList(0), ... 
				Do(*_ut, _resultUT);

				from = "CONCAT_ALL(std::vector<wiz::DataType>{ std::string() + " + PrintSomeUT(_resultUT) + "})";
			}
			{
				wiz::load_data::UserType* _ut = eventUT.GetUserTypeList(i)->GetUserTypeList(1);
				wiz::load_data::UserType _resultUT;
				// for item, item in ut->GetUserTypeList(0), ... 
				Do(*_ut, _resultUT);

				value = "CONCAT_ALL(std::vector<wiz::DataType>{ std::string() + " + PrintSomeUT(_resultUT) + "})";
			}

			{
				wiz::load_data::UserType _ut;
				as = eventUT.GetUserTypeList(i)->GetItemList(0).ToString();

				as = "CONCAT_ALL(std::vector<wiz::DataType>{ std::string() + \"" + as + "\" })";
			}
			result += "__func___assign_from_ut(*global, excuteData, locals, ";
			result += as;
			result += ", " + from + ", " + value + ");";

			result += "\n";
		}
		else if (functionName == "$option")
		{
			option = eventUT.GetUserTypeList(i)->ToString();
		}
		else if (functionName == "$assign") {
			result += "__func___assign(*global, excuteData, locals, \"";
			result += ToStr(wiz::ToString(eventUT.GetUserTypeList(i)->GetItemList(0).Get(0)));
			result += "\"";
			result += ", ";

			{
				wiz::load_data::UserType* ut = eventUT.GetUserTypeList(i)->GetUserTypeList(0);
				wiz::load_data::UserType resultUT;
				// for item, item in ut->GetUserTypeList(0), ... 
				Do(*ut, resultUT);

				result += "CONCAT_ALL(std::vector<wiz::DataType>{ std::string() + " + PrintSomeUT(resultUT) + "})";
			}

			if (eventUT.GetUserTypeList(i)->GetUserTypeListSize() >= 2) {
				wiz::load_data::UserType* _ut = eventUT.GetUserTypeList(i)->GetUserTypeList(1);
				wiz::load_data::UserType _resultUT;
				// for item, item in ut->GetUserTypeList(0), ... 
				Do(*_ut, _resultUT);

				result += ", CONCAT_ALL(std::vector<wiz::DataType>{ std::string() + " + PrintSomeUT(_resultUT) + "})";
			}
			else {
				result += ", \"TRUE\"";
			}
			// chk condition! - defualt "TRUE" ?, chk size!
			result += ");";

			result += "\n";
		}
		else if (functionName == "$assign2") {
			std::string dir, value, condition = "\"TRUE\"";

			{
				wiz::load_data::UserType* _ut = eventUT.GetUserTypeList(i)->GetUserTypeList(0);
				wiz::load_data::UserType _resultUT;
				// for item, item in ut->GetUserTypeList(0), ... 
				Do(*_ut, _resultUT);

				dir = "CONCAT_ALL(std::vector<wiz::DataType>{ std::string() + " + PrintSomeUT(_resultUT) + "})";
			}
			{
				wiz::load_data::UserType* _ut = eventUT.GetUserTypeList(i)->GetUserTypeList(1);
				wiz::load_data::UserType _resultUT;
				// for item, item in ut->GetUserTypeList(0), ... 
				Do(*_ut, _resultUT);

				value = "CONCAT_ALL(std::vector<wiz::DataType>{ std::string() + " + PrintSomeUT(_resultUT) + "})";
			}

			result += "__func___assign2(*global, excuteData, locals, ";
			result += dir + ", " + value + ", " + condition + ");";

			result += "\n";
		}
		else if (functionName == "$while") {
			result += "while (__expr___bool(";
			wiz::load_data::UserType* ut = eventUT.GetUserTypeList(i)->GetUserTypeList(0);
			wiz::load_data::UserType resultUT;
			// for item, item in ut->GetUserTypeList(0), ... 
			Do(*ut, resultUT);

			{
				result += PrintSomeUT(resultUT, true);
			}
			result += ")) {\n";
			result += ConvertFunction(global, _eventUT, *eventUT.GetUserTypeList(i)->GetUserTypeList(1), depth + 1, option, is_module, module_name);
			result += "\n";
			for (int i = 0; i < depth; ++i) {
				result += "\t";
			}
			result += "}\n";
		}
		else if (functionName == "$if") {
			result += "if (__expr___bool(";
			wiz::load_data::UserType* ut = eventUT.GetUserTypeList(i)->GetUserTypeList(0);
			wiz::load_data::UserType resultUT;
			// for item, item in ut->GetUserTypeList(0), ... 
			Do(*ut, resultUT);

			{
				std::string expr = PrintSomeUT(resultUT, true);

				result += expr;

				result += ")";
			}

			result += ") {\n";
			result += ConvertFunction(global, _eventUT, *eventUT.GetUserTypeList(i)->GetUserTypeList(1), depth + 1, option, is_module, module_name);
			result += "\n";
			for (int i = 0; i < depth; ++i) {
				result += "\t";
			}
			result += "}\n";
		}
		else if (functionName == "$else") {
			result += "else {\n";
			result += ConvertFunction(global, _eventUT, *eventUT.GetUserTypeList(i)->GetUserTypeList(0), depth + 1, option, is_module, module_name);
			result += "\n";
			for (int i = 0; i < depth; ++i) {
				result += "\t";
			}
			result += "}\n";
		}
		else if (functionName == "$insert"
			|| functionName == "$insert2"
			|| functionName == "$push_back") {
			std::string value;
			std::string dir;
			int num = 0;
			if (eventUT.GetUserTypeList(i)->GetItemListSize() > 0) {
				dir = "\"" + ToStr(wiz::ToString(eventUT.GetUserTypeList(i)->GetItemList(0).Get(0))) + "\"";
				//wiz::load_data::UserType _ut;
				//wiz::load_data::UserType resultUT;

				//wiz::load_data::LoadData::LoadDataFromString(dir, _ut);
				// for item, item in ut->GetUserTypeList(0), ... 
				//Do(_ut, resultUT);

				//dir = "CONCAT_ALL(std::vector<wiz::DataType>{ " + PrintSomeUT(resultUT) + "})";
			}
			else ///val->Ge
			{
				wiz::load_data::UserType* ut = eventUT.GetUserTypeList(i)->GetUserTypeList(0);
				wiz::load_data::UserType resultUT;
				// for item, item in ut->GetUserTypeList(0), ... 
				Do(*ut, resultUT);

				dir = "CONCAT_ALL(std::vector<wiz::DataType>{ std::string() + " + PrintSomeUT(resultUT) + "})";
				num++;
			}

			std::string condition = "\"TRUE\"";
			if (eventUT.GetUserTypeList(i)->GetUserTypeListSize() >= num + 1) {
				wiz::load_data::UserType* ut = eventUT.GetUserTypeList(i)->GetUserTypeList(num);
				wiz::load_data::UserType resultUT;
				// for item, item in ut->GetUserTypeList(0), ... 
				Do(*ut, resultUT);

				condition = "CONCAT_ALL(std::vector<wiz::DataType>{ std::string() +  " + PrintSomeUT(resultUT) + "})";
			}

			{
				wiz::load_data::UserType* ut = eventUT.GetUserTypeList(i)->GetUserTypeList(num);
				wiz::load_data::UserType resultUT;
				// for item, item in ut->GetUserTypeList(0), ... 
				Do(*ut, resultUT);

				value = "CONCAT_ALL(std::vector<wiz::DataType>{ std::string() +  " + PrintSomeUT(resultUT) + "})";
			}

			result += "__func___insert(*global, excuteData, " + (dir)+", " + (value)+", " + (condition)+"); \n";
		}
		else if (functionName == "$call") {
			// todo!
			// itemtype, usertype...
			// chk parameter!
			// chk event?
			wiz::load_data::UserType* func = eventUT.GetUserTypeList(i);
			std::string id;

			wiz::ArrayMap<std::string, std::string> param;

			for (int j = 0; j < func->GetItemListSize(); ++j) {
				if ("id" == wiz::ToString(func->GetItemList(j).GetName())) {
					id = ToStr(wiz::ToString(func->GetItemList(j).Get(0)));
				}
				else {
					std::string value = "\"" + ToStr(wiz::ToString(func->GetItemList(j).Get(0))) + "\"";
					//wiz::load_data::UserType _ut;
					//wiz::load_data::UserType resultUT;

					//wiz::load_data::LoadData::LoadDataFromString(value, _ut);
					/// for item, item in ut->GetUserTypeList(0), ... 
					//Do(_ut, resultUT);

					//value = "CONCAT_ALL(std::vector<wiz::DataType>{ " + PrintSomeUT(resultUT) + "})";

					param[wiz::ToString(func->GetItemList(j).GetName())] = value;
				}
			}
			for (int j = 0; j < func->GetUserTypeListSize(); ++j) {
				if ("id" == func->GetUserTypeList(j)->GetName()) {
					wiz::load_data::UserType* ut = func->GetUserTypeList(j);
					wiz::load_data::UserType resultUT;
					// for item, item in ut->GetUserTypeList(0), ... 
					Do(*ut, resultUT);

					id = PrintSomeUT(resultUT);
				}
				else {
					wiz::load_data::UserType* ut = func->GetUserTypeList(j);
					wiz::load_data::UserType resultUT;
					// for item, item in ut->GetUserTypeList(0), ... 
					Do(*ut, resultUT);
					std::cout << resultUT.ToString() << std::endl;

					param[wiz::ToString(ut->GetName())] = PrintSomeUT(resultUT);

					std::cout << param[wiz::ToString(ut->GetName())] << std::endl;
				}
			}

			result += "{\n";

			for (int i = 0; i < depth + 1; ++i) {
				result += "\t";
			}
			result += "wiz::ArrayMap<std::string, std::string> param;\n";
			for (const auto& x : param) {
				for (int i = 0; i < depth + 1; ++i) {
					result += "\t";
				}
				result += "param[\"" + x.first + "\"] = " + x.second + ";\n";
			}

			result += "\n";
			for (int i = 0; i < depth + 1; ++i) {
				result += "\t";
			}
			result += "auto x = ";

			if (!is_module) {
				id = "__event__" + id;
			}
			else {
				id = "__module_" + wiz::toStr(module_name.size()) + "_" + module_name + "_" + wiz::toStr(module_name.size()) + "_" + id;
			}

			result += id;
			result += "(global, excuteData, param);";
			result += "\n";

			for (int i = 0; i < depth + 1; ++i) {
				result += "\t";
			}

			result += "if (x.first) { result_change = true; result = x.second; }\n";

			if ("REMOVE_IF_CALL_ONESELF_EVENT" == option && _eventUT.GetItem("id")[0].Get(0) == id)
			{
				for (int i = 0; i < depth; ++i) {
					result += "\t";
				}
				result += "return { result_change, result };\n";
			}
			else if ("REMOVE_IF_CALL_ANY_EVENT" == option)
			{
				for (int i = 0; i < depth; ++i) {
					result += "\t";
				}
				result += "return { result_change, result };\n";
			}

			for (int i = 0; i < depth; ++i) {
				result += "\t";
			}
			result += "}\n";
		}
		else if (functionName == "$make") {
			std::string condition = "\"TRUE\"";

			result += "__func___make(*global, excuteData";

			if (eventUT.GetUserTypeList(i)->GetUserTypeListSize() > 0) {

				{
					wiz::load_data::UserType* _ut = eventUT.GetUserTypeList(i)->GetUserTypeList(0);
					wiz::load_data::UserType _resultUT;
					// for item, item in ut->GetUserTypeList(0), ... 
					Do(*_ut, _resultUT);

					result += ", CONCAT_ALL(std::vector<wiz::DataType>{ std::string() + " + PrintSomeUT(_resultUT) + "})";
				}

				if (eventUT.GetUserTypeList(i)->GetUserTypeListSize() >= 2) {
					wiz::load_data::UserType* _ut = eventUT.GetUserTypeList(i)->GetUserTypeList(1);
					wiz::load_data::UserType _resultUT;
					// for item, item in ut->GetUserTypeList(0), ... 
					Do(*_ut, _resultUT);

					condition = PrintSomeUT(_resultUT);
				}
			}
			else {
				std::string data;
				{
					std::string x = "\"" + ToStr(eventUT.GetUserTypeList(i)->ToString()) + "\"";
					//wiz::load_data::UserType _ut;
					//wiz::load_data::UserType _resultUT;

					//wiz::load_data::LoadData::LoadDataFromString(x, _ut);

					// for item, item in ut->GetUserTypeList(0), ... 
					//Do(_ut, _resultUT);

					data = (std::move(x)); //", CONCAT_ALL(std::vector<wiz::DataType>{ " + PrintSomeUT(_resultUT) + "})";
				}
				result += ", " + data;
			}

			result += ", " + condition + ");";

			result += "\n";
		}
		else if (functionName == "$return") {
			result += "result_change = true;\n";

			for (int k = 0; k < depth; ++k) {
				result += "\t";
			}

			result += "return ";
			std::string data;
			{
				std::string x = eventUT.GetUserTypeList(i)->ToString();

				std::cout << x << std::endl;

				wiz::load_data::UserType _ut;
				wiz::load_data::UserType _resultUT;

				wiz::load_data::LoadData::LoadDataFromString(x, _ut);

				// for item, item in ut->GetUserTypeList(0), ... 
				Do(_ut, _resultUT);

				data = "{ result_change, CONCAT_ALL(std::vector<wiz::DataType>{ std::string() +  " + PrintSomeUT(_resultUT, false, 0, 1) + "}) };";
			}
			result += data;

			result += "\n";
		}
		else if (functionName == "$setElement") {
			std::string dir, idx, value;
			{
				wiz::load_data::UserType* _ut = eventUT.GetUserTypeList(i)->GetUserTypeList(0);
				wiz::load_data::UserType _resultUT;
				// for item, item in ut->GetUserTypeList(0), ... 
				Do(*_ut, _resultUT);

				dir = "CONCAT_ALL(std::vector<wiz::DataType>{ std::string() + " + PrintSomeUT(_resultUT) + "})";
			}
			{
				wiz::load_data::UserType* _ut = eventUT.GetUserTypeList(i)->GetUserTypeList(1);
				wiz::load_data::UserType _resultUT;
				// for item, item in ut->GetUserTypeList(0), ... 
				Do(*_ut, _resultUT);

				idx = "CONCAT_ALL(std::vector<wiz::DataType>{ std::string() + " + PrintSomeUT(_resultUT) + "})";
			}
			{
				wiz::load_data::UserType* _ut = eventUT.GetUserTypeList(i)->GetUserTypeList(2);
				wiz::load_data::UserType _resultUT;
				// for item, item in ut->GetUserTypeList(0), ... 
				Do(*_ut, _resultUT);

				value = "CONCAT_ALL(std::vector<wiz::DataType>{ std::string() + " + PrintSomeUT(_resultUT) + "})";
			}

			result += "__func___setElement(*global, excuteData, " + dir + ", " + idx + ", " + value + ");";

			result += "\n";
		}
		else if (functionName == "$swap") {
			wiz::load_data::UserType* val = eventUT.GetUserTypeList(i);
			std::string dir = "\"" + ToStr(val->GetItemList(0).ToString()) + "\""; // + 0

			std::string value1;// = val->GetUserTypeList(0)->ToString();
			std::string value2;// = val->GetUserTypeList(1)->ToString();

			{
				wiz::load_data::UserType* _ut = eventUT.GetUserTypeList(i)->GetUserTypeList(0);
				wiz::load_data::UserType _resultUT;
				// for item, item in ut->GetUserTypeList(0), ... 
				Do(*_ut, _resultUT);

				value1 = "CONCAT_ALL(std::vector<wiz::DataType>{ std::string() + " + PrintSomeUT(_resultUT) + "})";
			}
			{
				wiz::load_data::UserType* _ut = eventUT.GetUserTypeList(i)->GetUserTypeList(1);
				wiz::load_data::UserType _resultUT;
				// for item, item in ut->GetUserTypeList(0), ... 
				Do(*_ut, _resultUT);

				value2 = "CONCAT_ALL(std::vector<wiz::DataType>{ std::string() +  " + PrintSomeUT(_resultUT) + "})";
			}
			result += "__func___swap(global, excuteData, " + dir + ", " + value1 + ", " + value2 + ");\n";

			result += "\n";
		}
		else if (functionName == "$findIndex") {
			std::string dir, value;
			{
				wiz::load_data::UserType* _ut = eventUT.GetUserTypeList(i)->GetUserTypeList(0);
				wiz::load_data::UserType _resultUT;
				// for item, item in ut->GetUserTypeList(0), ... 
				Do(*_ut, _resultUT);

				dir = "CONCAT_ALL(std::vector<wiz::DataType>{ std::string() + " + PrintSomeUT(_resultUT) + "})";
			}
			{
				wiz::load_data::UserType* _ut = eventUT.GetUserTypeList(i)->GetUserTypeList(1);
				wiz::load_data::UserType _resultUT;
				// for item, item in ut->GetUserTypeList(0), ... 
				Do(*_ut, _resultUT);

				value = "CONCAT_ALL(std::vector<wiz::DataType>{ std::string() + " + PrintSomeUT(_resultUT) + "})";
			}

			result += "__func___findIndex(global, excuteData, " + dir + ", " + value + ");\n";

			result += "\n";
		}
		else if (functionName == "$remove3") {
			std::string dir, value, condition = "\"TRUE\"";
			{
				wiz::load_data::UserType* _ut = eventUT.GetUserTypeList(i)->GetUserTypeList(0);
				wiz::load_data::UserType _resultUT;
				// for item, item in ut->GetUserTypeList(0), ... 
				Do(*_ut, _resultUT);

				dir = "CONCAT_ALL(std::vector<wiz::DataType>{ std::string() + " + PrintSomeUT(_resultUT) + "})";
			}
			{
				wiz::load_data::UserType* _ut = eventUT.GetUserTypeList(i)->GetUserTypeList(1);
				wiz::load_data::UserType _resultUT;
				// for item, item in ut->GetUserTypeList(0), ... 
				Do(*_ut, _resultUT);

				value = "CONCAT_ALL(std::vector<wiz::DataType>{ std::string() + " + PrintSomeUT(_resultUT) + "})";
			}
			if (eventUT.GetUserTypeList(i)->GetUserTypeListSize() >= 3) {
				wiz::load_data::UserType* _ut = eventUT.GetUserTypeList(i)->GetUserTypeList(2);
				wiz::load_data::UserType _resultUT;
				// for item, item in ut->GetUserTypeList(0), ... 
				Do(*_ut, _resultUT);

				condition = "CONCAT_ALL(std::vector<wiz::DataType>{ std::string() + " + PrintSomeUT(_resultUT) + "})";
			}

			result += "__func___remove3(global, excuteData, " + dir + ", " + value + ", " + condition + ");\n";
			result += "\n";
		}
		// $print is complicated! and has bug!
		else if (functionName == "$print") {
			wiz::load_data::UserType* val = eventUT.GetUserTypeList(i);

			if (val->GetUserTypeListSize() == 1
				&& val->GetUserTypeList(0)->GetItemListSize() == 1)
			{
				std::string listName = wiz::ToString(val->GetUserTypeList(0)->GetItemList(0).Get(0));

				if (listName.size() >= 2 && listName[0] == '\"' && listName.back() == '\"')
				{
					result += std::string("") + "std::cout << " + listName + ";\n";
				}
				else if (listName.size() == 2 && listName[0] == '\\' && listName[1] == 'n')
				{
					result += "std::cout << \"\\n\";\n";
				}
				else if (wiz::String::startsWith(listName, "$local.")
					|| wiz::String::startsWith(listName, "$parameter.")
					) {
					std::string temp;

					{
						wiz::load_data::UserType _ut;
						wiz::load_data::LoadData::LoadDataFromString(listName, _ut);
						wiz::load_data::UserType _resultUT;
						// for item, item in ut->GetUserTypeList(0), ... 
						Do(_ut, _resultUT);

						temp = "CONCAT_ALL(std::vector<wiz::DataType>{ std::string() + " + PrintSomeUT(_resultUT) + "})";
					}

					result += "if (" + temp + ".empty()) {\n";
					result += "std::cout << \"EMPTY\";\n";
					result += "}\n";
					result += "else {\n";
					result += "std::cout << " + temp + ";\n";
					result += "}\n";
				}
				else if (wiz::String::startsWith(listName, "/") && listName.size() > 1)
				{
					std::string temp;

					{
						wiz::load_data::UserType _ut;
						wiz::load_data::LoadData::LoadDataFromString(listName, _ut);
						wiz::load_data::UserType _resultUT;
						// for item, item in ut->GetUserTypeList(0), ... 
						Do(_ut, _resultUT);

						temp = "CONCAT_ALL(std::vector<wiz::DataType>{ std::string() + " + PrintSomeUT(_resultUT) + "})";
					}

					if (temp != listName) // chk 
					{
						result += "std::cout << " + temp + ";\n";
					}
					else {
						result += "{\n";
						result += "wiz::load_data::UserType* ut = wiz::load_data::UserType::Find(global, listName).second[0];\n";
						result += "if (ut->GetItemListSize() == 0 && wiz::ToString(ut->GetItemList(0).GetName()).empty()) {\n";
						result += "\tstd::cout << wiz::ToString(ut->GetItemList(0).Get(0));\n";
						result += "}\n";
						result += "}\n";
					}
				}
				else
				{
					result += "{\n";
					result += "auto x = wiz::load_data::UserType::Find(global, \"" + listName + "\");\n";
					result += "if (x.first) { \n";
					result += "wiz::load_data::UserType* ut = x.second[0];\n";
					result += "std::cout << ut->ToString();\n";
					result += "}\n";
					result += "else {\n";
					result += std::string("") + "std::cout << \"" + listName + "\";\n";
					result += "}\n";
					result += "}\n";
				}
			}
			// ?
			else if (val->GetUserTypeListSize() == 1
				&& val->GetUserTypeList(0)->GetItemListSize() == 0
				&& val->GetUserTypeList(0)->GetUserTypeListSize() == 1)
			{
				std::string temp = val->GetUserTypeList(0)->ToString();

				std::string name;
				{
					wiz::load_data::UserType _ut;
					wiz::load_data::LoadData::LoadDataFromString(temp, _ut);
					wiz::load_data::UserType _resultUT;
					// for item, item in ut->GetUserTypeList(0), ... 
					Do(_ut, _resultUT);

					name = "CONCAT_ALL(std::vector<wiz::DataType>{ std::string() + " + PrintSomeUT(_resultUT) + "})";
				}
				result += "std::cout << " + name + ";\n";
			}
			else
			{
				std::string start;
				std::string last;

				{
					wiz::load_data::UserType* _ut = eventUT.GetUserTypeList(i)->GetUserTypeList(1);
					wiz::load_data::UserType _resultUT;
					// for item, item in ut->GetUserTypeList(0), ... 
					Do(*_ut, _resultUT);

					start = "CONCAT_ALL(std::vector<wiz::DataType>{ std::string() + " + PrintSomeUT(_resultUT) + "})";
				}
				{
					wiz::load_data::UserType* _ut = eventUT.GetUserTypeList(i)->GetUserTypeList(2);
					wiz::load_data::UserType _resultUT;
					// for item, item in ut->GetUserTypeList(0), ... 
					Do(*_ut, _resultUT);

					last = "CONCAT_ALL(std::vector<wiz::DataType>{ std::string() + " + PrintSomeUT(_resultUT) + "})";
				}

				std::string listName = wiz::ToString(val->GetUserTypeList(0)->GetItemList(0).Get(0));
				{
					wiz::load_data::UserType _ut;
					wiz::load_data::LoadData::LoadDataFromString(listName, _ut);
					wiz::load_data::UserType _resultUT;
					// for item, item in ut->GetUserTypeList(0), ... 
					Do(_ut, _resultUT);

					listName = "CONCAT_ALL(std::vector<wiz::DataType>{ std::string() + " + PrintSomeUT(_resultUT) + "})";
				}

				result += "{\n";
				result += "int _start = (stoi(" + start + "))" + ";\n";
				result += "int _last = (stoi(" + last + "))" + ";\n";
				result += "wiz::load_data::UserType* ut = wiz::load_data::UserType::Find(global, " + listName + ").second[0];\n";
				result += "for (int i = _start; i <= _last; ++i)\n";
				result += "{\n";
				result += "if (i != _start) { std::cout << \" \"; } \n";
				result += "std::cout << wiz::ToString(ut->GetItemList(i).Get(0));\n";
				result += "}\n";
				result += "}\n";
			}

			result += "\n";
		}
		//
		else if (functionName == "$call_by_data") {
			std::string dir = eventUT.GetUserTypeList(i)->ToString();

			wiz::load_data::UserType _func;
			wiz::load_data::LoadData::LoadDataFromString(
				std::string("") + "$call = { id = " + "__module_" + wiz::toStr(dir.size()) + "_" + dir + "_" + wiz::toStr(dir.size()) + "_Main } ", _func);

			std::string id;
			wiz::load_data::UserType* func = _func.GetUserTypeList(0);

			wiz::ArrayMap<std::string, std::string> param;

			for (int j = 0; j < func->GetItemListSize(); ++j) {
				if ("id" == wiz::ToString(func->GetItemList(j).GetName())) {
					id = ToStr(wiz::ToString(func->GetItemList(j).Get(0)));
				}
				else {
					std::string value = "\"" + ToStr(wiz::ToString(func->GetItemList(j).Get(0))) + "\"";
					//wiz::load_data::UserType _ut;
					//wiz::load_data::UserType resultUT;

					//wiz::load_data::LoadData::LoadDataFromString(value, _ut);
					/// for item, item in ut->GetUserTypeList(0), ... 
					//Do(_ut, resultUT);

					//value = "CONCAT_ALL(std::vector<wiz::DataType>{ " + PrintSomeUT(resultUT) + "})";

					param[wiz::ToString(func->GetItemList(j).GetName())] = value;
				}
			}
			for (int j = 0; j < func->GetUserTypeListSize(); ++j) {
				if ("id" == func->GetUserTypeList(j)->GetName()) {
					wiz::load_data::UserType* ut = func->GetUserTypeList(j);
					wiz::load_data::UserType resultUT;
					// for item, item in ut->GetUserTypeList(0), ... 
					Do(*ut, resultUT);

					id = PrintSomeUT(resultUT);
				}
				else {
					wiz::load_data::UserType* ut = func->GetUserTypeList(j);
					wiz::load_data::UserType resultUT;
					// for item, item in ut->GetUserTypeList(0), ... 
					Do(*ut, resultUT);

					param[wiz::ToString(ut->GetName())] = PrintSomeUT(resultUT);
				}
			}

			result += "{\n";

			for (int i = 0; i < depth + 1; ++i) {
				result += "\t";
			}
			result += "wiz::ArrayMap<std::string, std::string> param;\n";
			for (const auto& x : param) {
				for (int i = 0; i < depth + 1; ++i) {
					result += "\t";
				}
				result += "param[\"" + x.first + "\"] = " + x.second + ";\n";
			}

			result += "\n";
			for (int i = 0; i < depth + 1; ++i) {
				result += "\t";
			}
			result += "auto x = ";
			result += id;
			result += "(global, excuteData, param);";
			result += "\n";

			for (int i = 0; i < depth + 1; ++i) {
				result += "\t";
			}

			result += "if (x.first) { result_change = true; result = x.second; }\n";

			result += "}\n";
		}
		else if (functionName == "$replace_datatype1_2") {
			result += "{\n";
			result += "std::string data = \"" + ToStr(eventUT.GetUserTypeList(i)->ToString()) + "\";\n";
			result += "wiz::load_data::UserType val;\n";
			result += "wiz::load_data::LoadData::LoadDataFromString(data, val);\n";
			result += "std::string rex = wiz::load_data::LoadData::ToBool4(nullptr, *global, val.GetUserTypeList(0)->ToString(), excuteData);\n";
			result += "rex = rex.substr(1, rex.size() - 2);\n";
			result += "std::vector<wiz::DataType> sval;\n";
			result += "std::vector<wiz::DataType> scondition;\n";
			result += "std::string start_dir = \"root\";\n";

			result += "if (val.GetUserTypeListSize() >= 3)\n";
			result += "{\n";
			result += "	const int n = (val.GetUserTypeListSize() - 3);\n";
			result += "	for (int i = 0; i < n; i = i + 2) { \n";
			result += "	sval.push_back(val.GetUserTypeList(1 + i)->ToString());\n";

			result += "	scondition.push_back(val.GetUserTypeList(2 + i)->ToString());\n";
			result += "	}\n";
			result += "}\n";

			result += "if (val.GetUserTypeListSize() >= 4) {\n";
			result += "start_dir = wiz::load_data::LoadData::ToBool4(nullptr, *global, val.GetUserTypeList(val.GetUserTypeListSize() - 2)->ToString(), excuteData);\n";
			result += "	}\n";
			result += "bool recursive = true;\n";
			result += "	if (val.GetUserTypeListSize() >= 5) {\n";
			result += "	recursive = wiz::load_data::LoadData::ToBool4(nullptr, *global, val.GetUserTypeList(val.GetUserTypeListSize() - 1)->ToString(), excuteData) == \"TRUE\" ? true : false;\n";
			result += "	}\n";

			result += "	wiz::load_data::LoadData::ReplaceDataType1_2(*global, rex, sval, scondition, start_dir, excuteData, recursive);\n";
			result += "}\n";
		}

		else if (functionName == "$iterate") { // re.. need fixed?
			// dir - no tobool4
			// event ids - no tobool4
			// (recursive : FALSE default?, use tobool4)
			std::string dir;
			std::vector<std::string> ids;
			std::string recursive = "FALSE";

			wiz::load_data::UserType* func = eventUT.GetUserTypeList(i);
			dir = ToStr(func->GetUserTypeList(0)->ToString());

			for (int j = 0; j < func->GetUserTypeList(1)->GetItemListSize(); ++j) {
				ids.push_back(ToStr(wiz::ToString(func->GetUserTypeList(1)->GetItemList(j).Get(0))));
			}
			for (int k = 0; k < ids.size(); ++k) {
				if (!is_module) {
					ids[k] = "__event__" + ids[k];
				}
				else {
					ids[k] = "__module_" + wiz::toStr(module_name.size()) + "_" + module_name + "_" + wiz::toStr(module_name.size()) + "_" + ids[k];
				}
			}
			if (func->GetUserTypeListSize() >= 3) {
				wiz::load_data::UserType* ut = func->GetUserTypeList(2);
				wiz::load_data::UserType resultUT;

				Do(*ut, resultUT);

				recursive = PrintSomeUT(resultUT);
			}

			for (int i = 0; i < depth + 1; ++i) {
				result += "\t";
			}
			result += "{\n";

			for (int i = 0; i < depth + 1; ++i) {
				result += "\t";
			}
			result += std::string() + "std::string dir = " + "\"" + ToStr(dir) + "\"" + ";\n";

			for (int i = 0; i < depth + 1; ++i) {
				result += "\t";
			}
			result += "std::vector<wiz::load_data::UserType*> dirUTVec = wiz::load_data::UserType::Find(global, dir).second;\n";

			for (int i = 0; i < depth + 1; ++i) {
				result += "\t";
			}
			result += "for (int t = 0; t < dirUTVec.size(); ++t) {\n";

			for (int i = 0; i < depth + 1; ++i) {
				result += "\t";
			}

			for (int i = 0; i < depth + 1; ++i) {
				result += "\t";
			}
			result += "Iterate(global, dirUTVec[t], ";
			result += "std::vector<std::pair<bool, wiz::DataType> (*)(wiz::load_data::UserType*, wiz::ExecuteData&, wiz::ArrayMap<std::string, std::string>&)>{ ";
			for (int j = 0; j < ids.size(); ++j) {
				result += ids[j];

				if (j < ids.size() - 1) {
					result += ", ";
				}
			}
			result += "}, ";
			result += recursive + ", " + wiz::_toString(depth) + ", " + (is_module ? "true" : "false");

			if (is_module) {
				result += ", " + module_name + ", " + "wiz::load_data::LoadData::GetRealDir(dir, dirUTVec[t])" + ", parameters" + ");\n";
			}
			else {
				result += std::string() + ", " + "\"empty_module_name\"" + ", " +
					"wiz::load_data::LoadData::GetRealDir(dir, dirUTVec[t])" + ", parameters" + ");\n";
			}

			for (int i = 0; i < depth + 1; ++i) {
				result += "\t";
			}
			result += "}\n";

			for (int i = 0; i < depth + 1; ++i) {
				result += "\t";
			}
			result += "}\n";
			result += "\n";
		}
		else if (functionName == "$iterate2") { // re.. need fixed?
			// dir - no tobool4
			// event ids - no tobool4
			// (recursive : FALSE default?, use tobool4)
			std::string dir;
			std::vector<std::string> ids;
			std::string recursive = "FALSE";

			wiz::load_data::UserType* func = eventUT.GetUserTypeList(i);
			dir = ToStr(func->GetUserTypeList(0)->ToString());

			for (int j = 0; j < func->GetUserTypeList(1)->GetItemListSize(); ++j) {
				ids.push_back(ToStr(wiz::ToString(func->GetUserTypeList(1)->GetItemList(j).Get(0))));
			}
			for (int k = 0; k < ids.size(); ++k) {
				if (!is_module) {
					ids[k] = "__event__" + ids[k];
				}
				else {
					ids[k] = "__module_" + wiz::toStr(module_name.size()) + "_" + module_name + "_" + wiz::toStr(module_name.size()) + "_" + ids[k];
				}
			}
			if (func->GetUserTypeListSize() >= 3) {
				wiz::load_data::UserType* ut = func->GetUserTypeList(2);
				wiz::load_data::UserType resultUT;

				Do(*ut, resultUT);

				recursive = PrintSomeUT(resultUT);
			}

			for (int i = 0; i < depth + 1; ++i) {
				result += "\t";
			}
			result += "{\n";

			for (int i = 0; i < depth + 1; ++i) {
				result += "\t";
			}
			result += std::string() + "std::string dir = " + "\"" + ToStr(dir) + "\"" + ";\n";

			for (int i = 0; i < depth + 1; ++i) {
				result += "\t";
			}
			result += "std::vector<wiz::load_data::UserType*> dirUTVec = wiz::load_data::UserType::Find(global, dir).second;\n";

			for (int i = 0; i < depth + 1; ++i) {
				result += "\t";
			}
			result += "for (int t = 0; t < dirUTVec.size(); ++t) {\n";

			for (int i = 0; i < depth + 1; ++i) {
				result += "\t";
			}

			for (int i = 0; i < depth + 1; ++i) {
				result += "\t";
			}
			result += "Iterate2(global, dirUTVec[t], ";
			result += "std::vector<std::pair<bool, wiz::DataType> (*)(wiz::load_data::UserType*, wiz::ExecuteData&, wiz::ArrayMap<std::string, std::string>&)>{ ";
			for (int j = 0; j < ids.size(); ++j) {
				result += ids[j];

				if (j < ids.size() - 1) {
					result += ", ";
				}
			}
			result += "}, ";
			result += recursive + ", " + wiz::_toString(depth) + ", " + (is_module ? "true" : "false");

			if (is_module) {
				result += ", " + module_name + ", " + "wiz::load_data::LoadData::GetRealDir(dir, dirUTVec[t])" + ", parameters" + ");\n";
			}
			else {
				result += std::string() + ", " + "\"empty_module_name\"" + ", " +
					"wiz::load_data::LoadData::GetRealDir(dir, dirUTVec[t])" + ", parameters" + ");\n";
			}

			for (int i = 0; i < depth + 1; ++i) {
				result += "\t";
			}
			result += "}\n";

			for (int i = 0; i < depth + 1; ++i) {
				result += "\t";
			}
			result += "}\n";
			result += "\n";
		}
		// todo ! 
		else if (functionName == "$save_data_only2") {
			// chk functionName?
			std::string dir_name;
			std::string file_name;
			std::string option;

			{
				wiz::load_data::UserType* _ut = eventUT.GetUserTypeList(i)->GetUserTypeList(0);
				wiz::load_data::UserType _resultUT;
				// for item, item in ut->GetUserTypeList(0), ... 
				Do(*_ut, _resultUT);

				dir_name = "CONCAT_ALL(std::vector<wiz::DataType>{ std::string() + " + PrintSomeUT(_resultUT) + "})";
			}
			{
				wiz::load_data::UserType* _ut = eventUT.GetUserTypeList(i)->GetUserTypeList(1);
				wiz::load_data::UserType _resultUT;
				// for item, item in ut->GetUserTypeList(0), ... 
				Do(*_ut, _resultUT);

				std::string name = PrintSomeUT(_resultUT);
				if (name.size() >= 6 && name[1] == '\\' && name[2] == '\"'
					&& name[name.size() - 3] == '\\' && name[name.size() - 2] == '\"') {
					name = "\"" + name.substr(3, name.size() - 6) + "\"";
				}
				file_name = "CONCAT_ALL(std::vector<wiz::DataType>{ std::string() + " + name + "})";
			}
			{
				wiz::load_data::UserType* _ut = eventUT.GetUserTypeList(i)->GetUserTypeList(2);
				wiz::load_data::UserType _resultUT;
				// for item, item in ut->GetUserTypeList(0), ... 
				Do(*_ut, _resultUT);

				option = "CONCAT_ALL(std::vector<wiz::DataType>{ std::string() + " + PrintSomeUT(_resultUT) + "})";
			}

			result += "__func___save_data_only2(global, excuteData, " + dir_name + ", " + file_name + ", " + option + ");\n";
			result += "\n";
		}
		else if (functionName == "$load_only_data2")
		{
			std::string file_name;
			std::string dir_name;
			std::string lex_thr_num = "0"; // 0 means use all thread.
			std::string parse_thr_num = "0";

			{
				wiz::load_data::UserType* _ut = eventUT.GetUserTypeList(i)->GetUserTypeList(0);
				wiz::load_data::UserType _resultUT;
				// for item, item in ut->GetUserTypeList(0), ... 
				Do(*_ut, _resultUT);

				std::string name = PrintSomeUT(_resultUT);

				if (name.size() >= 6 && name[1] == '\\' && name[2] == '\"'
					&& name[name.size() - 3] == '\\' && name[name.size() - 2] == '\"') {
					name = "\"" + name.substr(3, name.size() - 6) + "\"";
				}

				std::cout << "name is " << name << "\n";

				file_name = "CONCAT_ALL(std::vector<wiz::DataType>{ std::string() + " + name + "})";
			}
			{
				wiz::load_data::UserType* _ut = eventUT.GetUserTypeList(i)->GetUserTypeList(1);
				wiz::load_data::UserType _resultUT;
				// for item, item in ut->GetUserTypeList(0), ... 
				Do(*_ut, _resultUT);

				dir_name = "CONCAT_ALL(std::vector<wiz::DataType>{ std::string() + " + PrintSomeUT(_resultUT) + "})";
			}
			if (eventUT.GetUserTypeList(i)->GetUserTypeListSize() >= 3) {
				wiz::load_data::UserType* _ut = eventUT.GetUserTypeList(i)->GetUserTypeList(2);
				wiz::load_data::UserType _resultUT;
				// for item, item in ut->GetUserTypeList(0), ... 
				Do(*_ut, _resultUT);

				lex_thr_num = "CONCAT_ALL(std::vector<wiz::DataType>{ std::string() + " + PrintSomeUT(_resultUT) + "})";
			}
			if (eventUT.GetUserTypeList(i)->GetUserTypeListSize() >= 4) {
				wiz::load_data::UserType* _ut = eventUT.GetUserTypeList(i)->GetUserTypeList(3);
				wiz::load_data::UserType _resultUT;
				// for item, item in ut->GetUserTypeList(0), ... 
				Do(*_ut, _resultUT);

				parse_thr_num = "CONCAT_ALL(std::vector<wiz::DataType>{ std::string() + " + PrintSomeUT(_resultUT) + "})";
			}

			result += "__func___load_only_data2(global, excuteData, " + file_name + ", " + dir_name + ", " + lex_thr_num + ", " + parse_thr_num + ");\n";
			result += "\n";
		}
	}

	return result;
}

inline std::string RemoveQuoted(const std::string& str)
{
	if (str.size() >= 2 && str.front() == str.back() && (str.front() == '\"' || str.front() == '\''))
	{
		return str.substr(1, str.size() - 2);
	}
	return str;
}

int main(int argc, char* argv[])
{
	//try {
	int count_sequential = 0;
	int count_parallel = 0;
	int lex_parallel = 1;
	int parse_parallel = 1;

	wiz::load_data::UserType MainUT;
	std::vector<wiz::load_data::UserType*> EventUT;
	wiz::load_data::UserType global;
	std::string name;
	const std::string outputName = "test.cpp";
	std::ofstream outFile;

	outFile.open(outputName);
	if (false == outFile.is_open()) {
		return 1;
	}

	{ // part A
		std::ifstream inFile;
		inFile.open("functionA.txt");
		if (false == inFile.is_open()) {
			outFile.close();
			return 2;
		}

		// copy data from infile to outfile.
		std::string temp;

		while (std::getline(inFile, temp)) {
			outFile << temp << "\n";
		}

		// close
		inFile.close();
	}

	{ // part B
		if (1 == argc) {
			std::cout << "fileName : ";
			std::getline(std::cin, name);
		}
		else {
			name = std::string(argv[1]);
		}
		wiz::load_data::LoadData::LoadDataFromFile(name, global);
	}



	{ // part C
		auto x = global.GetUserTypeItem("Main");
		if (x.size() != 1) {
			return -1; //
		}
		MainUT = *x[0];

		// chk $load_only_data
		{
			auto x = MainUT.GetUserTypeItem("$load_only_data"); //
			if (!x.empty()) {
				wiz::load_data::UserType* val = nullptr;

				int count = 0;
				for (int i = 0; i < x.size(); ++i) {
					val = x[i];
					std::string dir = val->GetUserTypeList(1)->ToString();

					count_sequential++;

					wiz::load_data::UserType _global;

					wiz::load_data::LoadData::LoadDataFromFile(wiz::load_data::LoadData::ToBool4(nullptr, global,
						RemoveQuoted(val->GetUserTypeList(0)->ToString()), wiz::ExecuteData()).ToString(), _global);

					{
						int item_n = 0;
						int user_n = 0;
						wiz::load_data::UserType* ut;

						if (dir == "/./" || dir == "root") {
							ut = &global;
						}
						else {
							dir = wiz::load_data::LoadData::ToBool4(nullptr, global, dir, wiz::ExecuteData()).ToString();
							ut = global.GetUserTypeItem(dir)[0];
						}

						for (int k = 0; k < _global.GetIListSize(); ++k) {
							if (_global.IsItemList(k)) {
								ut->AddItem(wiz::ToString(_global.GetItemList(item_n).GetName()), wiz::ToString(_global.GetItemList(item_n).Get(0)));
								item_n++;
							}
							else {
								ut->AddUserTypeItem(*_global.GetUserTypeList(user_n));
								user_n++;
							}
						}
					}

					if (val->GetUserTypeListSize() >= 3 && val->GetUserTypeList(2)->ToString() == "USE_MODULE") {
						for (int j = 0; j < _global.GetUserTypeListSize(); ++j) {
							if (_global.GetUserTypeList(j)->GetName() != "Event") {
								if (_global.GetUserTypeList(j)->GetName() == "Main") {
									outFile << "INLINE std::pair<bool, wiz::DataType> " << "__module_" << dir.size();

									outFile << "_" << dir << "_" << dir.size() << "_" << "Main" << "(wiz::load_data::UserType"
										<< "* global, wiz::ExecuteData& excuteData, wiz::ArrayMap<std::string, std::string>& parameters);" << "\n";
								}
								continue;
							}
							const std::string event_name = wiz::ToString(_global.GetUserTypeList(j)->GetItem("id")[0].Get(0));

							//// check name dupplication? - 00 ~ 99?
							outFile << "INLINE std::pair<bool, wiz::DataType> ";
							outFile << "__module_" + wiz::toStr(dir.size()) + "_" + dir + "_" + wiz::toStr(dir.size());
							outFile << "_" << event_name << "(wiz::load_data::UserType"
								<< "* global, wiz::ExecuteData& excuteData, wiz::ArrayMap<std::string, std::string>& parameters);" << "\n";
						}
					}

					count++;
				}
			}
		}

		// chk $load_only_data2(parallel)
		{
			auto x = MainUT.GetUserTypeItem("$load_only_data2"); //
			if (!x.empty()) {
				wiz::load_data::UserType* val = nullptr;

				int count = 0;
				for (int i = 0; i < x.size(); ++i) {
					val = x[i];
					std::string dir = val->GetUserTypeList(1)->ToString();

					wiz::load_data::UserType _global;

					count_parallel++;

					int pivot_num = 0;
					int lex_thr_num = 0;

					if (val->GetUserTypeListSize() >= 3) {
						lex_thr_num = stoi(val->GetUserTypeList(2)->ToString());
					}
					if (val->GetUserTypeListSize() >= 4) {
						int parse_thr_num = stoi(val->GetUserTypeList(3)->ToString());
						pivot_num = parse_thr_num - 1;
					}

					lex_parallel = lex_thr_num;
					parse_parallel = pivot_num + 1;

					wiz::load_data::LoadData::LoadDataFromFile(wiz::load_data::LoadData::ToBool4(nullptr, global,
						RemoveQuoted(val->GetUserTypeList(0)->ToString()), wiz::ExecuteData()).ToString(), _global, pivot_num, lex_thr_num);

					{
						int item_n = 0;
						int user_n = 0;
						wiz::load_data::UserType* ut;

						if (dir == "/./" || dir == "root") {
							ut = &global;
						}
						else {
							dir = wiz::load_data::LoadData::ToBool4(nullptr, global, dir, wiz::ExecuteData()).ToString();
							ut = global.GetUserTypeItem(dir)[0];
						}

						for (int k = 0; k < _global.GetIListSize(); ++k) {
							if (_global.IsItemList(k)) {
								ut->AddItem(_global.GetItemList(item_n).GetName(), _global.GetItemList(item_n).Get(0));
								item_n++;
							}
							else {
								ut->AddUserTypeItem(*_global.GetUserTypeList(user_n));
								user_n++;
							}
						}
					}

					if (val->GetUserTypeListSize() >= 5 && val->GetUserTypeList(4)->ToString() == "USE_MODULE") {
						for (int j = 0; j < _global.GetUserTypeListSize(); ++j) {
							if (_global.GetUserTypeList(j)->GetName() != "Event") {
								if (_global.GetUserTypeList(j)->GetName() == "Main") {
									outFile << "INLINE std::pair<bool, wiz::DataType> " << "__module_" << dir.size();

									outFile << "_" << dir << "_" << dir.size() << "_" << "Main" << "(wiz::load_data::UserType"
										<< "* global, wiz::ExecuteData& excuteData, wiz::ArrayMap<std::string, std::string>& parameters);" << "\n";
								}
								continue;
							}
							const std::string event_name = wiz::ToString(_global.GetUserTypeList(j)->GetItem("id")[0].Get(0));

							//// check name dupplication? - 00 ~ 99 ??
							outFile << "INLINE std::pair<bool, wiz::DataType> ";
							outFile << "__module_" + wiz::toStr(dir.size()) + "_" + dir + "_" + wiz::toStr(dir.size());
							outFile << "_" << event_name << "(wiz::load_data::UserType"
								<< "* global, wiz::ExecuteData& excuteData, wiz::ArrayMap<std::string, std::string>& parameters);" << "\n";
						}
					}

					count++;
				}
			}
		}

		EventUT = global.GetUserTypeItem("Event");

		//global.RemoveUserTypeList("Main");
		//global.RemoveUserTypeList("Event");

		// user defined functions declaration?
		const std::string main_name = wiz::ToString(MainUT.GetUserTypeItem("$call")[0]->GetItem("id")[0].Get(0));
		outFile << "INLINE std::pair<bool, wiz::DataType> " << "__event__" << main_name << "(wiz::load_data::UserType"
			<< "* global, wiz::ExecuteData& excuteData, wiz::ArrayMap<std::string, std::string>& parameters);" << "\n";
		for (int i = 0; i < EventUT.size(); ++i) {
			const std::string event_name = wiz::ToString(EventUT[i]->GetItem("id")[0].Get(0));
			if (event_name == main_name) {
				continue;
			}
			outFile << "INLINE std::pair<bool, wiz::DataType> " << "__event__" << event_name << "(wiz::load_data::UserType"
				<< "* global, wiz::ExecuteData& excuteData, wiz::ArrayMap<std::string, std::string>& parameters);" << "\n";
		}
	}

	{ // part D
		std::string fileName = name + "_test.txt";


		wiz::load_data::LoadData::SaveWizDB(global, fileName, "1");

		std::cout << "fileName is " << fileName << std::endl;
		fileName = wiz::String::replace(fileName, "\\", "/");
		std::cout << "fileName is " << fileName << std::endl;

		outFile << "\n" << "int main(void)\n"
			<< "{\n"
			<< "\twiz::ExecuteData excuteData;\n"
			<< "\twiz::load_data::UserType global;\n"
			<< "\tsrand(time(NULL));\n"
			<< "\tstd::string fileName = "
			<< "\""

			<< fileName

			<< "\""
			<< ";\n";
		if (0 == count_sequential && count_parallel > 0) {
			outFile << "\twiz::load_data::LoadData::LoadDataFromFile3(fileName, global, " << parse_parallel - 1 << ", " << lex_parallel << ");\n";
		}
		else {
			outFile << "\twiz::load_data::LoadData::LoadDataFromFile(fileName, global);\n"; // cf) LoadDataFromFile3
		}
		outFile << "\n"
			<< "\twiz::load_data::UserType events;\n"
			<< "\tauto x = global.GetUserTypeItem(\"Event\");\n"
			<< "\tfor (int i = 0; i < x.size(); ++i) {\n"
			<< "\tevents.AddUserTypeItem(*x[i]);\n"
			<< "\t} "
			<< "\texcuteData.chkInfo = true;"
			<< "\texcuteData.pEvents = &events;"
			<< "\n\twiz::ArrayMap<std::string, std::string> parameters;\n"
			<< "\tstd::cout << " << "__event__" << wiz::ToString(MainUT.GetUserTypeItem("$call")[0]->GetItem("id")[0].Get(0))
			<< "(&global, excuteData, parameters).second.ToString() << std::endl;\n"
			<< "\treturn 0;\n"
			<< "}\n\n";
	}

	{ // part E
		const std::string main_name = wiz::ToString(MainUT.GetUserTypeItem("$call")[0]->GetItem("id")[0].Get(0));

		for (int i = 0; i < EventUT.size(); ++i) {
			const std::string event_name = wiz::ToString(EventUT[i]->GetItem("id")[0].Get(0));

			outFile << "INLINE std::pair<bool, wiz::DataType> " << "__event__" << event_name << "(wiz::load_data::UserType"
				<< "* global, wiz::ExecuteData& excuteData";
			//if (event_name != main_name) 
			{
				outFile << ", wiz::ArrayMap<std::string, std::string>& parameters";
			}
			outFile << ")" << "\n"
				<< "{\n"
				<< "\twiz::ArrayMap<std::string, std::string> locals;\n"
				<< "\twiz::DataType result;\n"
				<< "\tbool result_change = false;\n"
				<< "\tstd::string option;\n\n";

			// set locals
			{
				auto locals = EventUT[i]->GetUserTypeItem("$local");
				if (!locals.empty()) {
					for (int j = 0; j < locals[0]->GetItemListSize(); ++j) {
						outFile << "\tlocals[\"" << ToStr(wiz::ToString(locals[0]->GetItemList(j).Get(0))) << "\"] = \"\";\n";
					}
				}
			}
			// convertedFunction = ConvertFunction(*global, ut, excutedata, locals, parameters);
				// $local = { }, $option = { } $parameter = { } => pass!
			outFile << "\n" << ConvertFunction(&global, *EventUT[i], *EventUT[i]) << "\n";
			outFile << "\treturn { result_change, result };\n";
			outFile << "}\n";
		}
	}

	{ // Part F
		auto x = MainUT.GetUserTypeItem("$load_only_data"); //
		if (!x.empty()) {
			wiz::load_data::UserType* val = nullptr;

			int count = 0;
			for (int i = 0; i < x.size(); ++i) {
				val = x[i];
				std::string dir = val->GetUserTypeList(1)->ToString();

				if (val->GetUserTypeListSize() >= 3 && val->GetUserTypeList(2)->ToString() == "USE_MODULE") {

					wiz::load_data::UserType _global;

					wiz::load_data::LoadData::LoadDataFromFile(wiz::load_data::LoadData::ToBool4(nullptr, global,
						RemoveQuoted(val->GetUserTypeList(0)->ToString()), wiz::ExecuteData()).ToString(), _global);


					for (int j = 0; j < _global.GetUserTypeListSize(); ++j) {
						if (_global.GetUserTypeList(j)->GetName() != "Event") {
							if (_global.GetUserTypeList(j)->GetName() == "Main") {
								std::string event_name = "Main"; //_global.GetUserTypeList(j)->GetItem("id")[0].Get(0);

								//// check name dupplication? - 00 ~ 99
								event_name = std::string("") + "__module_" + wiz::toStr(dir.size()) + "_" + dir + "_" + wiz::toStr(dir.size()) + "_" + event_name;

								outFile << "INLINE std::pair<bool, wiz::DataType> " << event_name << "(wiz::load_data::UserType"
									<< "* _global, wiz::ExecuteData& excuteData"; // check!
								outFile << ", wiz::ArrayMap<std::string, std::string>& parameters";

								outFile << ")" << "\n"
									<< "{\n"
									<< "\twiz::ArrayMap<std::string, std::string> locals;\n"
									<< "\twiz::DataType result;\n"
									<< "\tbool result_change = false;\n"
									<< "\twiz::load_data::UserType* global = _global->GetUserTypeItem(\"" + dir + "\")[0];\n"
									<< "\tstd::string option;\n\n";

								// convertedFunction = ConvertFunction(*global, ut, excutedata, locals, parameters);
								// $local = { }, $option = { } $parameter = { } => pass!
								outFile << "\n" << ConvertFunction(&_global, *_global.GetUserTypeList(j), *_global.GetUserTypeList(j), 1, "", true, dir) << "\n";
								outFile << "\treturn { result_change, result };\n";
								outFile << "}\n";
							}
							continue;
						}
						std::string event_name = wiz::ToString(_global.GetUserTypeList(j)->GetItem("id")[0].Get(0));

						//// check name dupplication? - 00 ~ 99
						event_name = std::string("") + "__module_" + wiz::toStr(dir.size()) + "_" + dir + "_" + wiz::toStr(dir.size()) + "_" + event_name;

						outFile << "INLINE std::pair<bool, wiz::DataType> " << event_name << "(wiz::load_data::UserType"
							<< "* global, wiz::ExecuteData& excuteData";
						outFile << ", wiz::ArrayMap<std::string, std::string>& parameters";

						outFile << ")" << "\n"
							<< "{\n"
							<< "\twiz::ArrayMap<std::string, std::string> locals;\n"
							<< "\twiz::DataType result;\n"
							<< "\tbool result_change = false;\n"
							<< "\tstd::string option;\n\n";

						// set locals
						{
							auto locals = _global.GetUserTypeList(j)->GetUserTypeItem("$local");
							if (!locals.empty()) {
								for (int j = 0; j < locals[0]->GetItemListSize(); ++j) {
									outFile << "\tlocals[\"" << ToStr(wiz::ToString(locals[0]->GetItemList(j).Get(0))) << "\"] = \"\";\n";
								}
							}
						}
						// convertedFunction = ConvertFunction(*global, ut, excutedata, locals, parameters);
						// $local = { }, $option = { } $parameter = { } => pass!
						outFile << "\n" << ConvertFunction(&global, *_global.GetUserTypeList(j), *_global.GetUserTypeList(j), 1, "", true, dir) << "\n";
						outFile << "\treturn { result_change, result };\n";
						outFile << "}\n";
					}
				}
				count++;
			}
		}
	}

	{ // Part G
		// chk $load_only_data2(parallel)
		auto x = MainUT.GetUserTypeItem("$load_only_data2"); //
		if (!x.empty()) {
			wiz::load_data::UserType* val = nullptr;

			int count = 0;
			for (int i = 0; i < x.size(); ++i) {
				val = x[i];
				std::string dir = val->GetUserTypeList(1)->ToString();

				if (val->GetUserTypeListSize() >= 5 && val->GetUserTypeList(4)->ToString() == "USE_MODULE") {
					int pivot_num = 3;
					int lex_thr_num = 4;

					if (val->GetUserTypeListSize() >= 3) {
						lex_thr_num = stoi(val->GetUserTypeList(2)->ToString());
					}
					if (val->GetUserTypeListSize() >= 4) {
						int parse_thr_num = stoi(val->GetUserTypeList(3)->ToString());
						pivot_num = parse_thr_num - 1;
					}

					wiz::load_data::UserType _global;

					wiz::load_data::LoadData::LoadDataFromFile(wiz::load_data::LoadData::ToBool4(nullptr, global,
						RemoveQuoted(val->GetUserTypeList(0)->ToString()), wiz::ExecuteData()).ToString(), _global, pivot_num, lex_thr_num);


					for (int j = 0; j < _global.GetUserTypeListSize(); ++j) {
						if (_global.GetUserTypeList(j)->GetName() != "Event") {
							if (_global.GetUserTypeList(j)->GetName() == "Main") {
								std::string event_name = "Main"; //_global.GetUserTypeList(j)->GetItem("id")[0].Get(0);

																 //// check name dupplication? - 00 ~ 99
								event_name = std::string("") + "__module_" + wiz::toStr(dir.size()) + "_" + dir + "_" + wiz::toStr(dir.size()) + "_" + event_name;

								outFile << "INLINE std::pair<bool, wiz::DataType> " << event_name << "(wiz::load_data::UserType"
									<< "* _global, wiz::ExecuteData& excuteData"; // check!
								outFile << ", wiz::ArrayMap<std::string, std::string>& parameters";

								outFile << ")" << "\n"
									<< "{\n"
									<< "\twiz::ArrayMap<std::string, std::string> locals;\n"
									<< "\twiz::DataType result;\n"
									<< "\tbool result_change = false;\n"
									<< "\twiz::load_data::UserType* global = _global->GetUserTypeItem(\"" + dir + "\")[0];\n"
									<< "\tstd::string option;\n\n";

								// convertedFunction = ConvertFunction(*global, ut, excutedata, locals, parameters);
								// $local = { }, $option = { } $parameter = { } => pass!
								outFile << "\n" << ConvertFunction(&_global, *_global.GetUserTypeList(j), *_global.GetUserTypeList(j), 1, "", true, dir) << "\n";
								outFile << "\treturn { result_change, result };\n";
								outFile << "}\n";
							}
							continue;
						}
						std::string event_name = wiz::ToString(_global.GetUserTypeList(j)->GetItem("id")[0].Get(0));

						//// check name dupplication? - 00 ~ 99
						event_name = std::string("") + "__module_" + wiz::toStr(dir.size()) + "_" + dir + "_" + wiz::toStr(dir.size()) + "_" + event_name;

						outFile << "INLINE std::pair<bool, wiz::DataType> " << event_name << "(wiz::load_data::UserType"
							<< "* global, wiz::ExecuteData& excuteData";
						outFile << ", wiz::ArrayMap<std::string, std::string>& parameters";

						outFile << ")" << "\n"
							<< "{\n"
							<< "\twiz::ArrayMap<std::string, std::string> locals;\n"
							<< "\twiz::DataType result;\n"
							<< "\tbool result_change = false;\n"
							<< "\tstd::string option;\n\n";

						// set locals
						{
							auto locals = _global.GetUserTypeList(j)->GetUserTypeItem("$local");
							if (!locals.empty()) {
								for (int j = 0; j < locals[0]->GetItemListSize(); ++j) {
									outFile << "\tlocals[\"" << ToStr(wiz::ToString(locals[0]->GetItemList(j).Get(0))) << "\"] = \"\";\n";
								}
							}
						}
						// convertedFunction = ConvertFunction(*global, ut, excutedata, locals, parameters);
						// $local = { }, $option = { } $parameter = { } => pass!
						outFile << "\n" << ConvertFunction(&global, *_global.GetUserTypeList(j), *_global.GetUserTypeList(j), 1, "", true, dir) << "\n";
						outFile << "\treturn { result_change, result };\n";
						outFile << "}\n";
					}
				}
				count++;
			}
		}
	}

	// close
	outFile.close();
	//}
	//catch (...) {
	//	std::cout << "no expected result" << std::endl;
	//}

	return 0;
}
