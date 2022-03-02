#include "test_example_functions.h"

const double PRECISION_RELEVANCE = 0.00001;

namespace std {
	void AssertImpl(bool value, const string& expr_str, const string& file, const string& func, unsigned line,
		const string& hint) {
		if (!value) {
			cerr << file << "("s << line << "): "s << func << ": "s;
			cerr << "ASSERT("s << expr_str << ") failed."s;
			if (!hint.empty()) {
				cerr << " Hint: "s << hint;
			}
			cerr << endl;
			abort();
		}
	}

	void TestMatchingDoc() {
		const int doc_id = 42;
		const string content = "in cat tron text city "s;
		const vector<int> ratings = { 1, 2, 3 };
		//checking the return of all query words contained in the document
		{
			SearchServer server("in the"s);
			server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
			tuple<vector<string>, DocumentStatus> test = { {"cat", "tron"} , DocumentStatus::ACTUAL };
			ASSERT_HINT(server.MatchDocument("cat tron", 42) == test, "Error when adding words to Match Document");
		}
		//checking the return of all query words if there is a negative word contained in the document
		{
			SearchServer server("in the"s);
			server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
			tuple<vector<string>, DocumentStatus> test = { {} , DocumentStatus::ACTUAL };
			ASSERT(server.MatchDocument("cat -tron", 42) == test);
		}
	}

	void TestSortFromRelevance() {
		{   // checking sorting by relevance
			SearchServer server("in the"s);
			server.AddDocument(0, "����� ��� � ������ �������"s, DocumentStatus::ACTUAL, { 8, -3 });
			server.AddDocument(1, "�������� ��� �������� �����"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
			server.AddDocument(2, "��������� �� ������������� �����"s, DocumentStatus::ACTUAL, { 5, -12, 2, 1 });
			server.AddDocument(3, "������� ������� ���������"s, DocumentStatus::ACTUAL, { 9 });
			const auto found_docs = server.FindTopDocuments("�������� ��������� ���"s);
			const Document& doc0 = found_docs[0];
			const Document& doc1 = found_docs[1];
			const Document& doc2 = found_docs[2];
			const Document& doc3 = found_docs[3];
			ASSERT(doc0.relevance > doc1.relevance && doc1.relevance > doc2.relevance && doc2.relevance > doc3.relevance);
		}
	}

	void TestCalculateRElevance() {
		//checking the relevance calculation
		SearchServer server("in the"s);
		server.AddDocument(0, "�������� ��� �������� �����"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
		server.AddDocument(1, "�������� ���  �����"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
		server.AddDocument(2, "��������� �� ������������� �����"s, DocumentStatus::ACTUAL, { 5, -12, 2, 1 });
		server.AddDocument(3, "������� ������� ���������"s, DocumentStatus::ACTUAL, { 9 });
		const auto found_docs = server.FindTopDocuments("�������� ��������� ���"s);
		const Document& doc0 = found_docs[0];
		ASSERT((doc0.relevance - 0.51986) < PRECISION_RELEVANCE);
	}

	void TestCalculateRElevanceManyDocs() {
		//checking the relevance calculation with many docs
		SearchServer server("in the"s);
		server.AddDocument(0, "�������� ��� �������� �����"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
		server.AddDocument(1, "�������� ���  �����"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
		server.AddDocument(2, "��������� �� ������������� �����"s, DocumentStatus::ACTUAL, { 5, -12, 2, 1 });
		server.AddDocument(3, "������� �������  ������ �� ����� ���������"s, DocumentStatus::ACTUAL, { 9 });
		server.AddDocument(4, "�������� ��� ��������� �����"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
		server.AddDocument(5, "�������� ��� ��������� ������"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
		server.AddDocument(6, "��������� �� ������������� ����� ������� ��������"s, DocumentStatus::ACTUAL, { 5, -12, 2, 1 });
		server.AddDocument(7, "������� ������� ����� ��� � ������"s, DocumentStatus::ACTUAL, { 9 });
		const auto found_docs = server.FindTopDocuments("�������� ��������� ���"s);
		const Document& doc0 = found_docs[0];
		ASSERT((doc0.relevance - 0.46407) < 0.00001);
	}

	void TestSortWithPredicate() {
		//checkin sort with predicate
		SearchServer server("in the"s);
		server.AddDocument(0, "�������� ��� �������� �����"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
		server.AddDocument(1, "�������� ���  �����"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
		server.AddDocument(2, "��������� �� ������������� �����"s, DocumentStatus::ACTUAL, { 5, -12, 2, 1 });
		server.AddDocument(3, "������� ������� ���������"s, DocumentStatus::ACTUAL, { 9 });
		const auto found_docs = server.FindTopDocuments("�������� ��������� ���"s, [](int document_id, DocumentStatus status, int rating) { return document_id % 2 == 0; });
		ASSERT_EQUAL(found_docs.size(), 2);
	}

	void TestGetDocumentsForStatusIRRELEVANT() {
		SearchServer server("in the"s);
		server.AddDocument(0, "�������� ��� �������� �����"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
		server.AddDocument(1, "�������� ���  �����"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
		server.AddDocument(2, "��������� �� ������������� �����"s, DocumentStatus::ACTUAL, { 5, -12, 2, 1 });
		server.AddDocument(3, "������� ������� ���������"s, DocumentStatus::ACTUAL, { 9 });
		const auto found_docs = server.FindTopDocuments("�������� ��������� ���"s, DocumentStatus::IRRELEVANT);
		ASSERT(found_docs.size() == 0);
	}

	void TestGetDocumentsForStatusBANNED() {
		SearchServer server("in the"s);
		server.AddDocument(0, "�������� ��� �������� �����"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
		server.AddDocument(1, "�������� ���  �����"s, DocumentStatus::BANNED, { 7, 2, 7 });
		server.AddDocument(2, "��������� �� ������������� �����"s, DocumentStatus::ACTUAL, { 5, -12, 2, 1 });
		server.AddDocument(3, "������� ������� ���������"s, DocumentStatus::ACTUAL, { 9 });
		const auto found_docs = server.FindTopDocuments("�������� ��������� ���"s, DocumentStatus::BANNED);
		ASSERT_EQUAL(found_docs.size(), 1);
	}

	void TestGetDocumentsForStatusACTUAL_MAX_COUNT() {
		SearchServer server("in the"s);
		server.AddDocument(0, "�������� ��� �������� �����"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
		server.AddDocument(1, "�������� ���  �����"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
		server.AddDocument(2, "��������� �� ������������� �����"s, DocumentStatus::ACTUAL, { 5, -12, 2, 1 });
		server.AddDocument(3, "������� �������  ������ �� ����� ���������"s, DocumentStatus::ACTUAL, { 9 });
		server.AddDocument(4, "�������� ��� ��������� �����"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
		server.AddDocument(5, "�������� ��� ��������� ������"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
		server.AddDocument(6, "��������� �� ������������� ����� ������� ��������"s, DocumentStatus::ACTUAL, { 5, -12, 2, 1 });
		server.AddDocument(7, "������� ������� ����� ��� � ������"s, DocumentStatus::ACTUAL, { 9 });
		const auto found_docs = server.FindTopDocuments("�������� ��������� ���"s, DocumentStatus::ACTUAL);
		ASSERT(found_docs.size() == 5);
	}

	void TestSearchServer() {
		TestMatchingDoc();
		TestSortFromRelevance();
		TestCalculateRElevance();
		TestCalculateRElevanceManyDocs();
		TestSortWithPredicate();
		TestGetDocumentsForStatusIRRELEVANT();
		TestGetDocumentsForStatusBANNED();
		TestGetDocumentsForStatusACTUAL_MAX_COUNT();
	}
}