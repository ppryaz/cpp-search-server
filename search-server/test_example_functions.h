#pragma once

#include <string>
#include "search_server.h"

namespace std {
	template <typename T, typename U>
	void AssertEqualImpl(const T& t, const U& u, const string& t_str, const string& u_str, const string& file,
		const string& func, unsigned line, const string& hint) {
		if (t != u) {
			cout << boolalpha;
			cout << file << "("s << line << "): "s << func << ": "s;
			cout << "ASSERT_EQUAL("s << t_str << ", "s << u_str << ") failed: "s;
			cout << t << " != "s << u << "."s;
			if (!hint.empty()) {
				cout << " Hint: "s << hint;
			}
			cout << endl;
			abort();
		}
	}

#define ASSERT_EQUAL(a, b) AssertEqualImpl((a), (b), #a, #b, __FILE__, __FUNCTION__, __LINE__, ""s)
#define ASSERT_EQUAL_HINT(a, b, hint) AssertEqualImpl((a), (b), #a, #b, __FILE__, __FUNCTION__, __LINE__, (hint))

	void AssertImpl(bool value, const string& expr_str, const string& file, const string& func, unsigned line, const string& hint);

#define ASSERT(expr) AssertImpl(!!(expr), #expr, __FILE__, __FUNCTION__, __LINE__, ""s)
#define ASSERT_HINT(expr, hint) AssertImpl(!!(expr), #expr, __FILE__, __FUNCTION__, __LINE__, (hint))

	template <class TestFunc>
	void RunTestImpl(TestFunc t_func, const string& func) {
		t_func();
		cerr << func << endl;
	}

#define RUN_TEST(func) RunTestImpl((func),#func)

	void TestMatchingDoc();
	void TestSortFromRelevance();
	void TestCalculateRElevance();
	void TestCalculateRElevanceManyDocs();
	void TestSortWithPredicate();
	void TestGetDocumentsForStatusIRRELEVANT();
	void TestGetDocumentsForStatusBANNED();
	void TestGetDocumentsForStatusACTUAL_MAX_COUNT();
	//Run all test functions
	void TestSearchServer();
}