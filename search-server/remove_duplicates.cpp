#include "search_server.h"
#include "remove_duplicates.h"
#include <set>

using namespace std;

void RemoveDuplicates(SearchServer& search_server) {
	set <int> del;
	for (const int document_id_first : search_server) {
		set<string> first;
		for (auto& [key, value] : search_server.GetWordFrequencies(document_id_first)) {
			first.insert(key);
		}
		for (const int document_id_second : search_server) {
			if (document_id_second > document_id_first) {
				set<string> second;
				for (auto& [key, value] : search_server.GetWordFrequencies(document_id_second)) {
					second.insert(key);
				}
				if ((document_id_first != document_id_second) && (first == second)) {
					cout << "Found duplicate document id " << document_id_second << endl;
					del.insert(document_id_second);
				}
			}
		}
	}
	for (int id_doc : del) {
		search_server.RemoveDocument(id_doc);
	}
}
