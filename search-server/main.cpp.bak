// The test verifies that the search engine excludes stop words when adding documents
void TestExcludeStopWordsFromAddedDocumentContent() {
	const int doc_id = 42;
	const string content = "cat in the city"s;
	const vector<int> ratings = { 1, 2, 3 };
	{
		SearchServer server;
		server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
		const auto found_docs = server.FindTopDocuments("in"s);
		ASSERT_EQUAL(found_docs.size(), 1u);
		const Document& doc0 = found_docs[0];
		ASSERT_EQUAL(doc0.id, doc_id);
	}

	{
		SearchServer server;
		server.SetStopWords("in the"s);
		server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
		ASSERT_HINT(server.FindTopDocuments("in"s).empty(), "Stop words must be excluded from documents"s);
	}
}

void TestExcludeStopAndMinusWordsFromAddedDocumentContent() {
	const int doc_id = 42;
	const string content = "in cat the city "s;
	const vector<int> ratings = { 1, 2, 3 };
	// search for a word that is not included in the list of stop words,
    // finds the desired document
	{
		SearchServer server;
		server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
		const auto found_docs = server.FindTopDocuments("in"s);
		ASSERT_EQUAL(found_docs.size(), 1);
		const Document& doc0 = found_docs[0];
		ASSERT_EQUAL(doc0.id, doc_id);
	}
	// Then we make sure that the search for the same word included in the list of stop words
    // returns an empty result
	{
		SearchServer server;
		server.SetStopWords("in the"s);
		server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
		ASSERT(server.FindTopDocuments("in"s).empty());
	}
	//Checking for the rejection of a document containing a negative word
	{
		SearchServer server;
		server.SetStopWords("in the"s);
		server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
		ASSERT(server.FindTopDocuments("cash"s).empty());
	}
}

void TestMatchingDoc() {
	const int doc_id = 42;
	const string content = "in cat tron text city "s;
	const vector<int> ratings = { 1, 2, 3 };
	//checking the return of all query words contained in the document
	{
		SearchServer server;
		server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
		tuple<vector<string>, DocumentStatus> test = { {"cat", "tron"} , DocumentStatus::ACTUAL };
		ASSERT_HINT(server.MatchDocument("cat tron", 42) == test, "Error when adding words to Match Document");
	}
	//checking the return of all query words if there is a negative word contained in the document
	{
		SearchServer server;
		server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
		tuple<vector<string>, DocumentStatus> test = { {} , DocumentStatus::ACTUAL };
		ASSERT(server.MatchDocument("cat -tron", 42) == test);
	}
}

void TestCalculationRating() {
	const int doc_id = 42;
	const string content = "in cat tron text city "s;
	const vector<int> ratings = { 20, 8, 31 };
	{
		// checking calculation rating
		SearchServer server;
		server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
		const auto found_docs = server.FindTopDocuments("in"s);
		const Document& doc0 = found_docs[0];
		ASSERT(doc0.rating == 19);
	}
}

void TestCalculationRatingNegativNumbers() {
	const int doc_id = 42;
	const string content = "in cat tron text city "s;
	const vector<int> ratings = { 20,  -11, -21 };
	{
		// checking calculation rating
		SearchServer server;
		server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
		const auto found_docs = server.FindTopDocuments("in"s);
		const Document& doc0 = found_docs[0];
		ASSERT(doc0.rating == -4);
	}
}

void TestCalculationLotsOfRating() {
	const int doc_id = 42;
	const string content = "in cat tron text city "s;
	const vector<int> ratings = { 20,  -11, -21, 12, 31, 42, 21, 31,3 ,7 ,9 };
	{
		// checking calculation rating
		SearchServer server;
		server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
		const auto found_docs = server.FindTopDocuments("in"s);
		const Document& doc0 = found_docs[0];
		ASSERT(doc0.rating == 13);
	}
}

void TestSortFromRelevance() {
	{   // checking sorting by relevance
		SearchServer server;
		server.AddDocument(0, "белый кот и модный ошейник"s, DocumentStatus::ACTUAL, { 8, -3 });
		server.AddDocument(1, "пушистый кот пушистый хвост"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
		server.AddDocument(2, "ухоженный пёс выразительные глаза"s, DocumentStatus::ACTUAL, { 5, -12, 2, 1 });
		server.AddDocument(3, "скворец евгений ухоженный"s, DocumentStatus::ACTUAL, { 9 });
		const auto found_docs = server.FindTopDocuments("пушистый ухоженный кот"s);
		const Document& doc0 = found_docs[0];
		const Document& doc1 = found_docs[1];
		const Document& doc2 = found_docs[2];
		const Document& doc3 = found_docs[3];
		ASSERT(doc0.relevance > doc1.relevance && doc1.relevance > doc2.relevance && doc2.relevance > doc3.relevance);

	}
}

void TestCalculateRelevance() {
	//checking the relevance calculation
	SearchServer server;
	server.AddDocument(0, "пушистый кот пушистый хвост"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
	server.AddDocument(1, "пушистый кот  хвост"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
	server.AddDocument(2, "ухоженный пёс выразительные глаза"s, DocumentStatus::ACTUAL, { 5, -12, 2, 1 });
	server.AddDocument(3, "скворец евгений ухоженный"s, DocumentStatus::ACTUAL, { 9 });
	const auto found_docs = server.FindTopDocuments("пушистый ухоженный кот"s);
	const Document& doc0 = found_docs[0];
	ASSERT((doc0.relevance - 0.51986) < 0.00001);
}

void TestCalculateRelevanceManyDocs() {
	//checking the relevance calculation with many docs
	SearchServer server;
	server.AddDocument(0, "пушистый кот пушистый хвост"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
	server.AddDocument(1, "пушистый кот  хвост"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
	server.AddDocument(2, "ухоженный пёс выразительные глаза"s, DocumentStatus::ACTUAL, { 5, -12, 2, 1 });
	server.AddDocument(3, "скворец евгений  найден на улице ухоженный"s, DocumentStatus::ACTUAL, { 9 });
	server.AddDocument(4, "пушистый кот полосатый хвост"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
	server.AddDocument(5, "пушистый кот сибирской породы"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
	server.AddDocument(6, "ухоженный пёс выразительные глаза ошейник кожанный"s, DocumentStatus::ACTUAL, { 5, -12, 2, 1 });
	server.AddDocument(7, "скворец евгений рядом кот и собака"s, DocumentStatus::ACTUAL, { 9 });
	const auto found_docs = server.FindTopDocuments("пушистый ухоженный кот"s);
	const Document& doc0 = found_docs[0];
	const Document& doc1 = found_docs[1];
	const Document& doc2 = found_docs[2];
	const Document& doc4 = found_docs[4];
	ASSERT((doc0.relevance - 0.46407) < 0.00001);
	ASSERT((doc1.relevance - 0.387717) < 0.00001);
	ASSERT((doc2.relevance - 0.290788) < 0.00001);
	ASSERT((doc4.relevance - 0.245207) < 0.00001);
}

void TestSortWithPredicate() {
	//checkin sort with predicate
	SearchServer server;
	server.AddDocument(0, "пушистый кот пушистый хвост"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
	server.AddDocument(1, "пушистый кот  хвост"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
	server.AddDocument(2, "ухоженный пёс выразительные глаза"s, DocumentStatus::ACTUAL, { 5, -12, 2, 1 });
	server.AddDocument(3, "скворец евгений ухоженный"s, DocumentStatus::ACTUAL, { 9 });
	const auto found_docs = server.FindTopDocuments("пушистый ухоженный кот"s, [](int document_id, DocumentStatus status, int rating) { return document_id % 2 == 0; });
	ASSERT_EQUAL(found_docs.size(), 2);
}

void TestGetDocumentsForStatusIRRELEVANT() {
	SearchServer server;
	server.AddDocument(0, "пушистый кот пушистый хвост"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
	server.AddDocument(1, "пушистый кот  хвост"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
	server.AddDocument(2, "ухоженный пёс выразительные глаза"s, DocumentStatus::ACTUAL, { 5, -12, 2, 1 });
	server.AddDocument(3, "скворец евгений ухоженный"s, DocumentStatus::ACTUAL, { 9 });
	const auto found_docs = server.FindTopDocuments("пушистый ухоженный кот"s, DocumentStatus::IRRELEVANT);
	ASSERT(found_docs.size() == 0);
}

void TestGetDocumentsForStatusBANNED() {
	SearchServer server;
	server.AddDocument(0, "пушистый кот пушистый хвост"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
	server.AddDocument(1, "пушистый кот  хвост"s, DocumentStatus::BANNED, { 7, 2, 7 });
	server.AddDocument(2, "ухоженный пёс выразительные глаза"s, DocumentStatus::ACTUAL, { 5, -12, 2, 1 });
	server.AddDocument(3, "скворец евгений ухоженный"s, DocumentStatus::ACTUAL, { 9 });
	const auto found_docs = server.FindTopDocuments("пушистый ухоженный кот"s, DocumentStatus::BANNED);
	ASSERT_EQUAL(found_docs.size(), 1);
}

void TestGetDocumentsForStatusACTUAL_MAX_COUNT() {
	SearchServer server;
	server.AddDocument(0, "пушистый кот пушистый хвост"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
	server.AddDocument(1, "пушистый кот  хвост"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
	server.AddDocument(2, "ухоженный пёс выразительные глаза"s, DocumentStatus::ACTUAL, { 5, -12, 2, 1 });
	server.AddDocument(3, "скворец евгений  найден на улице ухоженный"s, DocumentStatus::ACTUAL, { 9 });
	server.AddDocument(4, "пушистый кот полосатый хвост"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
	server.AddDocument(5, "пушистый кот сибирской породы"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
	server.AddDocument(6, "ухоженный пёс выразительные глаза ошейник кожанный"s, DocumentStatus::ACTUAL, { 5, -12, 2, 1 });
	server.AddDocument(7, "скворец евгений рядом кот и собака"s, DocumentStatus::ACTUAL, { 9 });
	const auto found_docs = server.FindTopDocuments("пушистый ухоженный кот"s, DocumentStatus::ACTUAL);
	ASSERT(found_docs.size() == 5);
}

void TestSearchServer() {
	TestExcludeStopAndMinusWordsFromAddedDocumentContent();
	TestMatchingDoc();
	TestCalculationRating();
	TestCalculationRatingNegativNumbers();
	TestCalculationLotsOfRating();
	TestSortFromRelevance();
	TestCalculateRelevance();
	TestCalculateRelevanceManyDocs();
	TestSortWithPredicate();
	TestGetDocumentsForStatusIRRELEVANT();
	TestGetDocumentsForStatusBANNED();
	TestGetDocumentsForStatusACTUAL_MAX_COUNT();
}