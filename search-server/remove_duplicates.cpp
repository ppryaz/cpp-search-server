#include "search_server.h"
#include "remove_duplicates.h"

#include <set>

using namespace std;

void RemoveDuplicates(SearchServer& search_server) {
	set <int> id_dublicate_delete;
	map<set<string>, int>set_words_in_document;
	map<string, double>words_freq;
	set<string>words;
	for (const int document_id : search_server) {
		words_freq = search_server.GetWordFrequencies(document_id);
		for (auto [word, freq] : words_freq) {
			words.insert(word);
		}
		if (set_words_in_document.empty() || !set_words_in_document.count(words)) {
			set_words_in_document.insert(std::make_pair(words, document_id));
		}
		else {
			id_dublicate_delete.insert(document_id);
		}
		words_freq.clear();
	}
	for (int id_doc : id_dublicate_delete) {
		search_server.RemoveDocument(id_doc);
		cout << "Found duplicate document id " << id_doc << endl;
	}
}
