#pragma once

#include <deque>
#include <cmath>
#include "search_server.h"

namespace std {
	class RequestQueue {
	public:
		explicit RequestQueue(const SearchServer& search_server);
		//"обертки" для всех методов поиска, чтобы сохранять результаты для нашей статистики
		template <typename DocumentPredicate>
		vector<Document> AddFindRequest(const string& raw_query, DocumentPredicate document_predicate);
		vector<Document> AddFindRequest(const string& raw_query, DocumentStatus status);
		vector<Document> AddFindRequest(const string& raw_query);
		int GetNoResultRequests() const { return no_results_requests_; }
	private:
		struct QueryResult {
			uint64_t timestamp;
			int results;
		};
		std::deque<QueryResult> requests_;
		const SearchServer& search_server_;
		int no_results_requests_;
		uint64_t current_time_;
		const static int min_in_day_ = 1440;

		void AddRequest(int results_num);
	};

	template <typename DocumentPredicate>
	vector<Document> RequestQueue::AddFindRequest(const string& raw_query, DocumentPredicate document_predicate) {
		const auto result = search_server_.FindTopDocuments(raw_query, document_predicate);
		AddRequest(result.size());
		return result;
	}
}