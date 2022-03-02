#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>`
#include <vector>
#include <deque>

#include "search_server.h"
#include "request_queue.h"
#include "log_duration.h"
#include "remove_duplicates.h"
#include "test_example_functions.h"
#include "paginator.h"
using namespace std;

ostream& operator<<(ostream& out, const Document& document) {
	out << "{ "s
		<< "document_id = "s << document.id << ", "s
		<< "relevance = "s << document.relevance << ", "s
		<< "rating = "s << document.rating << " }"s;
	return out;
}

void PrintDocument(const Document& document) {
	cout << "{ "s
		<< "document_id = "s << document.id << ", "s
		<< "relevance = "s << document.relevance << ", "s
		<< "rating = "s << document.rating << " }"s << endl;
}

void PrintMatchDocumentResult(int document_id, const vector<string>& words, DocumentStatus status) {
	cout << "{ "s
		<< "document_id = "s << document_id << ", "s
		<< "status = "s << static_cast<int>(status) << ", "s
		<< "words ="s;
	for (const string& word : words) {
		cout << ' ' << word;
	}
	cout << "}"s << endl;
}

void AddDocument(SearchServer& search_server, int document_id, const string& document, DocumentStatus status,
	const vector<int>& ratings) {
	try {
		search_server.AddDocument(document_id, document, status, ratings);
	}
	catch (const invalid_argument& e) {
		cout << "Ошибка добавления документа "s << document_id << ": "s << e.what() << endl;
	}
}

void FindTopDocuments(const SearchServer& search_server, const string& raw_query) {
	cout << "Результаты поиска по запросу: "s << raw_query << endl;
	try {
		for (const Document& document : search_server.FindTopDocuments(raw_query)) {
			PrintDocument(document);
		}
	}
	catch (const invalid_argument& e) {
		cout << "Ошибка поиска: "s << e.what() << endl;
	}
}

void MatchDocuments(const SearchServer& search_server, const string& query) {
	try {
		cout << "Матчинг документов по запросу: "s << query << endl;
		//const int document_count = search_server.GetDocumentCount();
		for (const int document_id : search_server) {
			const auto [words, status] = search_server.MatchDocument(query, document_id);
			PrintMatchDocumentResult(document_id, words, status);
		}
	}
	catch (const invalid_argument& e) {
		cout << "Ошибка матчинга документов на запрос "s << query << ": "s << e.what() << endl;
	}
}

int main() {
	setlocale(LC_ALL, "rus");
	RUN_TEST(TestSearchServer);
	// Если вы видите эту строку, значит все тесты прошли успешно
	cout << "Search server testing finished"s << endl;

	SearchServer search_server("and in at"s);
	RequestQueue request_queue(search_server);

	search_server.AddDocument(1, "curly cat curly tail"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
	search_server.AddDocument(2, "curly dog and fancy collar"s, DocumentStatus::ACTUAL, { 1, 2, 3 });
	search_server.AddDocument(3, "big cat fancy collar "s, DocumentStatus::ACTUAL, { 1, 2, 8 });
	search_server.AddDocument(4, "big dog sparrow Eugene"s, DocumentStatus::ACTUAL, { 1, 3, 2 });
	search_server.AddDocument(5, "big dog sparrow Vasiliy"s, DocumentStatus::ACTUAL, { 1, 1, 1 });


	// 1439 запросов с нулевым результатом
	for (int i = 0; i < 1439; ++i) {
		request_queue.AddFindRequest("empty request"s);
	}
	// все еще 1439 запросов с нулевым результатом
	request_queue.AddFindRequest("curly dog"s);
	request_queue.AddFindRequest("sparrow"s);

	// новые сутки, первый запрос удален, 1438 запросов с нулевым результатом
	request_queue.AddFindRequest("big collar"s);
	// первый запрос удален, 1437 запросов с нулевым результатом
	{ LOG_DURATION_STREAM("Operation time: ", cerr);
	FindTopDocuments(search_server, "big dog"s); }
	
	{ LOG_DURATION_STREAM("Operation time: ", cerr);
	FindTopDocuments(search_server, "curly dog"s); }
	
	{ LOG_DURATION_STREAM("Operation time: ", cerr);
	MatchDocuments(search_server, "big dog"s); }
	
	{ LOG_DURATION_STREAM("Operation time: ", cerr);
	MatchDocuments(search_server, "curly dog"s); }
	
	cout << "Total empty requests: "s << request_queue.GetNoResultRequests() << endl;

	{LOG_DURATION_STREAM("Operation time: ", cerr);
	SearchServer search_server("and with"s);

	AddDocument(search_server, 1, "funny pet and nasty rat"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
	AddDocument(search_server, 2, "funny pet with curly hair"s, DocumentStatus::ACTUAL, { 1, 2 });
	// дубликат документа 2, будет удалён
	AddDocument(search_server, 3, "funny pet with curly hair"s, DocumentStatus::ACTUAL, { 1, 2 });
	// отличие только в стоп-словах, считаем дубликатом
	AddDocument(search_server, 4, "funny pet and curly hair"s, DocumentStatus::ACTUAL, { 1, 2 });

	// множество слов такое же, считаем дубликатом документа 1
	AddDocument(search_server, 5, "funny funny pet and nasty nasty rat"s, DocumentStatus::ACTUAL, { 1, 2 });

	// добавились новые слова, дубликатом не является
	AddDocument(search_server, 6, "funny pet and not very nasty rat"s, DocumentStatus::ACTUAL, { 1, 2 });

	// множество слов такое же, как в id 6, несмотря на другой порядок, считаем дубликатом
	AddDocument(search_server, 7, "very nasty rat and not very funny pet"s, DocumentStatus::ACTUAL, { 1, 2 });

	// есть не все слова, не является дубликатом
	AddDocument(search_server, 8, "pet with rat and rat and rat"s, DocumentStatus::ACTUAL, { 1, 2 });

	// слова из разных документов, не является дубликатом
	AddDocument(search_server, 9, "nasty rat with curly hair"s, DocumentStatus::ACTUAL, { 1, 2 });

	cout << "Before duplicates removed: "s << search_server.GetDocumentCount() << endl;
	RemoveDuplicates(search_server);
	cout << "After duplicates removed: "s << search_server.GetDocumentCount() << endl;
	}
}
