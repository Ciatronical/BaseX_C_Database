
/*
 : Copyright (c) 2014-2015 Jean-Marc Mercier, www.crimere.com
 : Licensed under the Apache License, Version 2.0 (the "License");
 : you may not use this file except in compliance with the License.
 : You may obtain a copy of the License at
 :
 :     http://www.apache.org/licenses/LICENSE-2.0
 :
 : Unless required by applicable law or agreed to in writing, software
 : distributed under the License is distributed on an "AS IS" BASIS,
 : WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 : See the License for the specific language governing permissions and
 : limitations under the License.
*/
#include "stdafx.h"
#include "string"
#include "iostream"
#include <list>
#include "BaseXClient.h"
#include "BaseXInterface.h"
#include <type_traits>
#include <typeinfo>
#include <corecrt_wstring.h>
#include <boost/locale.hpp>
#include <cstdio>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <clocale>

//#pragma execution_character_set( "UTF-8" )

using namespace std;
using namespace BaseXClient;
using namespace boost::locale;
using namespace boost::locale::conv;


struct my_int_list : std::list<string>
{};

struct my_int_list_interface : public BaseXInterface<my_int_list>
{
	typedef BaseXInterface<my_int_list> base_type;

	virtual ~my_int_list_interface() {};

	my_int_list_interface(const std::string& DBHOST, const std::string& DBPORT, const std::string& DBUSER,const std::string& DBPASSWD)  : base_type(DBHOST,DBPORT,DBUSER,DBPASSWD) {};
	virtual my_int_list get(string query,int stage){

		my_int_list my_int_list_;
		pugi::xml_encoding::encoding_utf16;
		pugi::xml_parse_result parse_;
		pugi::xml_document doc;
		

		//std::string results = session().execute("XQUERY <list> {for $i in 1 to 15 return <i>{$i}</i>}</list>");

		//std::string results = session().execute("XQUERY for $mid in db:open('Inter-Data','MIDANDLINKS/BMW_Links.xml')//MID	where $mid / Category = '33098'	order by $mid ascending	return $mid / Category ");
		
		//std::string results = session().execute("XQUERY for $mid in doc('C:/XML_DB/Inter-Data/MIDANDLINKS/BMW_Links.xml')//MID/id('BMW37457')	order by $mid ascending	return $mid");

		//std::string results = session().execute("XQUERY for $mid in doc('C:/XML_DB/Inter-Data/kbalinks_02_08_2016_XML.xml')//record,	$mid2 in doc('C:/XML_DB/Inter-Data/middata_V49_4 BMW ONLY_XML.xml')//record	where $mid / mid = 'BMW26527' and $mid2 / mid = 'BMW26527'	order by $mid ascending	return $mid");

		string results = session().execute("XQUERY "+query);

		//string results = session().execute("XQUERY for $item in doc('C:/XML_DB/Inter-Data/SCRIPTS/GER/wa_ger.xml')//ITEM[@ID=2] return $item/TEXTLEFT");
		//stage = 6;

		//cout << results << endl;
		parse_ = doc.load(results.c_str());
		//cout << "Load XML Input File : " << parse_.description() << std::endl;

		
		
		pugi::xpath_node_set is = doc.select_nodes("/record/mid");
		if(stage==2)
		is = doc.select_nodes("/Link");
		if (stage == 3)
			is = doc.select_nodes("/ADB/ITEM");
		if (stage == 4) {

			size_t pos = 0;
			string delimiter = "\n";
			std::string token;
			while ((pos = results.find(delimiter)) != std::string::npos) {
				token = results.substr(0, pos);
				
				my_int_list_.push_back(token);

				results.erase(0, pos + delimiter.length());
			}
			my_int_list_.push_back(results);
			return  my_int_list_;

		}if (stage == 5) {

			is = doc.select_nodes("/Variant");

		}

	if (stage == 6) {

		is = doc.select_nodes("/TEXTLEFT");

	}

		for (pugi::xpath_node_set::const_iterator it = is.begin(); it != is.end(); ++it)
		{
			 my_int_list_.push_back(it->node().text().as_string());
		};

		return  my_int_list_;
	};
};




list<string> hsn_tsn_pugixml(string hsn, string tsn)
{
	int stage = 1;
	string query = "for $mid in doc('C:/XML_DB/Inter-Data/kbalinks_02_08_2016_XML.xml')//record	where $mid / hsn = '"+hsn+"' and $mid / tsn = '"+tsn+"'	order by $mid ascending	return $mid";
	cout << "------------------------- " << endl;
	std::string DBHOST("127.0.0.1");
	std::string DBPORT("1984");
	std::string DBUSER("admin");
	std::string DBPASSWD("admin");
	my_int_list_interface list_interface(DBHOST, DBPORT, DBUSER, DBPASSWD);
	my_int_list list_ = list_interface.get(query, stage);
	
	list<string> mid;

	

	BOOST_FOREACH(string i, list_) {
			cout << "MID : " << i.c_str() << endl;
			
			mid.push_back(i);
	};

	return mid;
};


list<string> mid_pugixml(string mid)
{
	
	
	list<string> wam;
	int stage = 2;
	string query = "for $mid in doc('C:/XML_DB/Inter-Data/MIDANDLINKS/BMW_Links.xml')//MID[@ID='"+mid+"']//Category[@ID='WAM'] return $mid/Link";
	
	cout << "------------------------- " << endl;
	std::string DBHOST("127.0.0.1");
	std::string DBPORT("1984");
	std::string DBUSER("admin");
	std::string DBPASSWD("admin");
	my_int_list_interface list_interface(DBHOST, DBPORT, DBUSER, DBPASSWD);
	my_int_list list_ = list_interface.get(query, stage);

	

	BOOST_FOREACH(string i, list_) {
		
		//cout << "WAM : " << i << endl;
		
		wam.push_back(i);

	};

	cout << "------------------------- " << endl;

	if (list_.size()>1) {
	
		string query = "for $vari in doc('C:/XML_DB/Inter-Data/MIDANDLINKS/BMW_Links.xml')//MID[@ID='" + mid + "']//Category[@ID='WAM'] return data($vari/Link/@*)";
		my_int_list Vlist_ = list_interface.get(query, 4);
		int count = 1;
		BOOST_FOREACH(string i, Vlist_) {
			cout << " "<< count << " : Variant ID : " << i << endl;

			i.erase(remove_if(i.begin(), i.end(), isspace), i.end());

			string query = "for $info in doc('C:/XML_DB/Inter-Data/MISCELLANEOUS/GER/varia_ger.xml')/AllVariants/Variant[@ID='"+i+"'] return $info";
			my_int_list Ilist_ = list_interface.get(query, 5);
			BOOST_FOREACH(string x, Ilist_) {

				cout << " " << " : Variant : " << x << endl;

			}
			cout << "------------------------- " << endl;
			count++;
		};
	
	
	}



	return wam;
};



list<string> wam_data_pugixml(string wam)
{
	list<string> data;
	int stage = 3;

	if (wam.length() < 5)
		wam = "0" + wam;

	string query = "for $wam in doc('C:/XML_DB/Inter-Data/WHEEL_ALIGNMENT/DATA/"+wam+".xml')//WAMDATA/ADB	return $wam";
	cout << "------------------------- " << endl;
	std::string DBHOST("127.0.0.1");
	std::string DBPORT("1984");
	std::string DBUSER("admin");
	std::string DBPASSWD("admin");
	my_int_list_interface list_interface(DBHOST, DBPORT, DBUSER, DBPASSWD);
	my_int_list list_ = list_interface.get(query, stage);
	int count = 1;
	BOOST_FOREACH(string i, list_) {
		cout << count;
		string query = "for $item in doc('C:/XML_DB/Inter-Data/SCRIPTS/GER/wa_ger.xml')//ITEM[@ID='"+to_string(count)+"'] return $item/TEXTLEFT";
		my_int_list ITEMlist_ = list_interface.get(query, 6);
		BOOST_FOREACH(string x, ITEMlist_) {

			std::cout <<" : "<< x << " ";
		};

		
		std::cout << " : " << i << endl;
		data.push_back(i);
		count++;
	};

	
	return data;
};



list<string> manuf_pugixml(string manuf, string model)
{
	list<string> data;
	int stage = 1;

	
	string query = "for $manuf in doc('C:/XML_DB/Inter-Data/middata_V49_4 BMW ONLY_XML.xml')//record where $manuf/manufacturer='"+manuf+"'  and $manuf/model='"+model+"' return $manuf";
	cout << "------------------------- " << endl;
	std::string DBHOST("127.0.0.1");
	std::string DBPORT("1984");
	std::string DBUSER("admin");
	std::string DBPASSWD("admin");
	my_int_list_interface list_interface(DBHOST, DBPORT, DBUSER, DBPASSWD);
	my_int_list list_ = list_interface.get(query, stage);
	BOOST_FOREACH(string i, list_) {

		std::cout << "value : " << i << endl;
		data.push_back(i);
	};

	return data;
};


list<string> getManuf_pugixml()
{
	list<string> manuf;
	


	string query = "distinct-values(for $man in doc('C:/XML_DB/Inter-Data/middata_V49_4 BMW ONLY_XML.xml')//record return $man/manufacturer)";
	cout << "------------------------- " << endl;
	std::string DBHOST("127.0.0.1");
	std::string DBPORT("1984");
	std::string DBUSER("admin");
	std::string DBPASSWD("admin");
	my_int_list_interface list_interface(DBHOST, DBPORT, DBUSER, DBPASSWD);
	my_int_list list_ = list_interface.get(query, 4);
	int count = 1;
	BOOST_FOREACH(string i, list_) {

		std::cout << " : " << count <<" : "<< i << endl;
		manuf.push_back(i);
		count++;
	};

	return manuf;
};

list<string> getModel_pugixml(string manuf)
{
	list<string> model;



	string query = "distinct-values(for $model in doc('C:/XML_DB/Inter-Data/middata_V49_4 BMW ONLY_XML.xml')//record where $model/manufacturer = '"+manuf+"'	return $model/model)";
	cout << "------------------------- " << endl;
	std::string DBHOST("127.0.0.1");
	std::string DBPORT("1984");
	std::string DBUSER("admin");
	std::string DBPASSWD("admin");
	my_int_list_interface list_interface(DBHOST, DBPORT, DBUSER, DBPASSWD);
	my_int_list list_ = list_interface.get(query, 4);
	int count = 1;
	BOOST_FOREACH(string i, list_) {

		std::cout << " : " << count << " : " << i << endl;
		model.push_back(i);
		count++;
	};

	return model;
};



 /*void getWamData() {


	 if (midList.size()<2)
		 wamList = mid_pugixml(midList.front());
	 else {
		 std::cout << "------------------------- " << endl;
		 std::cout << "There are more variants please chose one(1-9):" << endl;
		 int count = 1;
		 BOOST_FOREACH(string i, midList) {
			 cout << count << " : MID : " << i << endl;
			 count++;
		 };
		 int select = 1;
		 cin >> select;
		 count = 1;
		 BOOST_FOREACH(string i, midList) {
			 if (select == count)
				 mid = i;
			 count++;
		 };


		 cout << mid << endl;
		 wamList = mid_pugixml(mid);
	 }
	 if (wamList.size()<2)
		 dataList = wam_data_pugixml(wamList.front());
	 else {

		 cout << "------------------------- " << endl;
		 cout << "There are more variants please chose one(1-9):" << endl;
		 int count = 1;
		 BOOST_FOREACH(string i, wamList) {
			 cout << " Variant : " << count << " : WAM : " << i << endl;
			 count++;
		 };
		 int select = 1;
		 cin >> select;
		 count = 1;
		 BOOST_FOREACH(string i, wamList) {
			 if (select == count)
				 wam = i;
			 count++;
		 };


		 dataList = wam_data_pugixml(wam);

	 }

 }




}*/


int main() {

	
	
	//std::setlocale(LC_ALL, "de_DE.UTF-16");
	std::locale::global(std::locale("German_germany"));
	//std::cout << "Hallo, schöne Welt!" << std::endl;
	//std::locale::global(std::locale("English_usa"));
	
	//cout << "ä";


	string hsn,tsn;
	int option;
	cout << "Search by manufacturer and vehicle type(1) or hsn and tsn(2) ?" << endl;
	cin >>option;

	if (option == 1) {
	cout << "-----------search by manufacturer and model-------------- " << endl;

	cout << "Please select manufacturer:" << endl;
	string manuf, model;
	list<string> manufList=getManuf_pugixml();

	int select = 1;
	cin >> select;
	int count = 0;
	BOOST_FOREACH(string i, manufList) {

		count++;

		if (select == count) {
			i.erase(remove_if(i.begin(), i.end(), isspace), i.end());
			manuf = i;
		}
		
	};
	cout << manuf;
	getModel_pugixml(manuf);
	
	cout << "Please enter model:" << endl;
	cin >> model;
	list<string> midList;

	midList=manuf_pugixml(manuf,model);



	}

	if (option == 2) {
		std::cout << "-----------search by hsn and tsn-------------- " << endl;

		cout << "Please enter hsn number:" << endl;
		cin >> hsn;
		cout << "Please enter tsn number:" << endl;
		cin >> tsn;
		string mid, wam;
		list<string> midList, dataList,wamList;
		midList = hsn_tsn_pugixml(hsn, tsn);
		if (midList.size()<2)
			wamList = mid_pugixml(midList.front());
		else {
			cout << "------------------------- " << endl;
			cout << "There are more variants please chose one(1-9):" << endl;
			int count = 1;
			BOOST_FOREACH(string i, midList) {
				cout << count << " : MID : " << i << endl;
				count++;
			};
			int select = 1;
			cin >> select;
			count = 1;
			BOOST_FOREACH(string i, midList) {
				if (select == count)
					mid = i;
				count++;
			};


			cout << mid << endl;
			wamList = mid_pugixml(mid);
		}
		if (wamList.size()<2)
		dataList = wam_data_pugixml(wamList.front());
		else {

			cout << "------------------------- " << endl;
			cout << "There are more variants please chose one(1-9):" << endl;
			int count = 1;
			BOOST_FOREACH(string i, wamList) {
				cout << " Variant : " << count << " : WAM : " << i << endl;
				count++;
			};
			int select = 1;
			cin >> select;
			count = 1;
			BOOST_FOREACH(string i, wamList) {
				if (select == count)
					wam = i;
				count++;
			};


			dataList = wam_data_pugixml(wam);
			
		}

	}




	return 0;
}



