#ifdef _DEBUG

#include <algorithm>
#include <execution>
#include <vector>

#include "search_server.h"
#include "testlib.h"



void TestIsValidWord() {
    {
        const std::vector<char> symbols_fail = { '\0', '\1' , '\037', '\036' };

        for (char symbol : symbols_fail) {
            ASSERT_HINT(!IsValidWord(std::string(1, symbol)), "IsValidWord is working incorrectly");
        }
    }

    {
        const std::vector<char> symbols_pass = { ' ', '!', '\376', '\377' };

        for (char symbol : symbols_pass) {
            ASSERT_HINT(IsValidWord(std::string(1, symbol)), "IsValidWord is working incorrectly");
        }
    }
}

void TestMakeNonEmptySet() {
    ASSERT_HINT(MakeNonEmptySet(std::vector<std::string>({})) == (std::set<std::string, std::less<>>({})),
        "MakeNonEmptySet doesn't work correctly");

    ASSERT_HINT(MakeNonEmptySet(std::vector<std::string>({ "", "" })) == (std::set<std::string, std::less<>>({})),
        "MakeNonEmptySet doesn't work correctly");

    ASSERT_HINT(MakeNonEmptySet(
        std::vector<std::string>({ "normal", "split" })) ==
        (std::set<std::string, std::less<>>({ "normal", "split" })),
        "MakeNonEmptySet doesn't work correctly");

    ASSERT_HINT(MakeNonEmptySet(
        std::vector<std::string>({ "", "spacy", "", "split" })) ==
        (std::set<std::string, std::less<>>({ "spacy", "split" })),
        "MakeNonEmptySet doesn't work correctly");
}

void TestSplitIntoWords() {
    ASSERT_HINT(SplitIntoWords("") == std::vector<std::string>({}),
        "SplitIntoWords doesn't work correctly");

    ASSERT_HINT(SplitIntoWords("          ") == std::vector<std::string>({}),
        "SplitIntoWords doesn't work correctly");

    ASSERT_HINT(SplitIntoWords("normal text") == std::vector<std::string>({ "normal", "text" }),
        "SplitIntoWords doesn't work correctly");

    ASSERT_HINT(SplitIntoWords("       weird      text   ") == std::vector<std::string>({ "weird", "text" }),
        "SplitIntoWords doesn't work correctly");

    ASSERT_HINT(SplitIntoWords("also  weird text") == std::vector<std::string>({ "also", "weird", "text" }),
        "SplitIntoWords doesn't work correctly");

}

void TestServerConstructor() {
    {
        try
        {
            SearchServer server1("a b c");
            SearchServer server2(std::set<std::string>({ "a", "b", "c" }));
            SearchServer server3(std::vector<std::string>({ "a", "b", "c" }));
        }
        catch (const std::exception&)
        {
            ASSERT_HINT(false, "Stop words validation is not working correctly");
        }

        try
        {
            SearchServer server("b\a c");
            ASSERT_HINT(false, "Stop words validation is not working correctly");
        }
        catch (const std::exception&) {}

        try
        {
            SearchServer server(std::set<std::string>({ "a", "b\xf", "c" }));
            ASSERT_HINT(false, "Stop words validation is not working correctly");
        }
        catch (const std::exception&) {}

        try
        {
            SearchServer server(std::vector<std::string>({ "a", "b", "c\b" }));
            ASSERT_HINT(false, "Stop words validation is not working correctly");
        }
        catch (const std::exception&) {}
    }
}

void TestDocumentAdding() {
    const std::vector<int> ratings = { 1 };
    const std::string valid_content1 = "hi hello";
    const std::string valid_content2 = "hilo";
    const std::string invalid_content1 = "hi h\12llo";
    const std::string invalid_content2 = "hl\2lo";

    {
        SearchServer server("");

        try
        {
            server.AddDocument(0, valid_content1, DocumentStatus::kActual, ratings);
            server.AddDocument(2, valid_content2, DocumentStatus::kActual, ratings);

        }
        catch (const std::exception&) {
            ASSERT_HINT(false, "AddDocument is not working correctly");
        }

        try
        {
            server.AddDocument(-1, valid_content1, DocumentStatus::kActual, ratings);
            ASSERT_HINT(false, "AddDocument must not allow negative document id");
        }
        catch (const std::exception&) {}

        try
        {
            server.AddDocument(0, valid_content1, DocumentStatus::kActual, ratings);
            ASSERT_HINT(false, "AddDocument must not allow adding a document with already existing document_id");
        }
        catch (const std::exception&) {}

        try
        {
            server.AddDocument(1, invalid_content1, DocumentStatus::kActual, ratings);
            ASSERT_HINT(false, "AddDocument must not allow illegal characters in the content");
        }
        catch (const std::exception&) {}

        try
        {
            server.AddDocument(1, invalid_content2, DocumentStatus::kActual, ratings);
            ASSERT_HINT(false, "AddDocument must not allow illegal characters in the content");
        }
        catch (const std::exception&) {}
    }

    {
        SearchServer server("");

        server.AddDocument(0, "cat in the city", DocumentStatus::kActual, { 1 });

        try
        {
            server.FindTopDocuments("c\xat");
            ASSERT_HINT(false, "FindTopDocuments must not allow illegal characters");
        }
        catch (const std::exception&) {}

        try
        {
            server.FindTopDocuments("cat --in");
            ASSERT_HINT(false, "FindTopDocuments must not allow minus word with second minus");
        }
        catch (const std::exception&) {}

        try
        {
            server.FindTopDocuments("cat -");
            ASSERT_HINT(false, "FindTopDocuments must not allow empty minus words");
        }
        catch (const std::exception&) {}
    }

    {
        const int document_id = 42;
        const std::string content1 = "cat in the city";
        const std::string content2 = "dog was found";

        {
            SearchServer server("");

            server.AddDocument(document_id, content1, DocumentStatus::kActual, ratings);
            server.AddDocument(document_id + 1, content2, DocumentStatus::kActual, ratings);

            ASSERT_EQUAL_HINT(server.GetDocumentCount(), 2, "AddDocument doesn't add documents properly");

            {
                const std::vector<Document>& result = server.FindTopDocuments(content1);

                ASSERT_EQUAL_HINT(result.size(), 1, "Document doesn't match itself");
                ASSERT_EQUAL_HINT(result[0].id, document_id, "Document content matches content of other document");
            }

            {
                const std::vector<Document>& result = server.FindTopDocuments(content2);

                ASSERT_EQUAL_HINT(result.size(), 1, "Document doesn't match itself");
                ASSERT_EQUAL_HINT(result[0].id, (document_id + 1),
                    "Document content matches content of other document");

            }

            ASSERT_HINT(server.FindTopDocuments("nothing here").empty(),
                "FindTopDocuments matches documents it must not match");
        }
    }

    {
        SearchServer server("");

        const std::string document = "testing things";

        for (int i = 0; i < 10; ++i) {
            server.AddDocument(i, document, DocumentStatus::kActual, ratings);
        }

        ASSERT_EQUAL_HINT(server.FindTopDocuments("things", DocumentStatus::kActual).size(), 5,
            "FindTopDocuments must return 5 documents at max");
    }
}

void TestDocumentCount() {
    const std::vector<int> ratings = { 1 };
    {
        SearchServer server("");

        ASSERT_EQUAL_HINT(server.GetDocumentCount(), 0, "Server should be empty at initialization");

        server.AddDocument(1, "a", DocumentStatus::kActual, ratings);

        ASSERT_EQUAL_HINT(server.GetDocumentCount(), 1, "GetDocumentCount is not counting correctly");

        server.AddDocument(2, "b", DocumentStatus::kActual, ratings);
        server.AddDocument(3, "c", DocumentStatus::kActual, ratings);
        server.AddDocument(4, "d", DocumentStatus::kActual, ratings);

        ASSERT_EQUAL_HINT(server.GetDocumentCount(), 4, "GetDocumentCount is not counting correctly");

        server.AddDocument(5, "e", DocumentStatus::kActual, ratings);
        server.AddDocument(6, "f", DocumentStatus::kActual, ratings);

        ASSERT_EQUAL_HINT(server.GetDocumentCount(), 6, "GetDocumentCount is not counting correctly");
    }
}

void TestStopWords() {
    const int document_id = 42;
    const std::vector<int> ratings = { 1 };

    {
        try {
            SearchServer server("\1");
            ASSERT_HINT(false, "SearchServer doesn't validate stop words correctly");
        }
        catch (const std::exception&) {}
    }

    {
        try {
            SearchServer server("\37");
            ASSERT_HINT(false, "SearchServer doesn't validate stop words correctly");
        }
        catch (const std::exception&) {}
    }

    {
        SearchServer server("in");

        server.AddDocument(document_id, "cat in the city", DocumentStatus::kActual, ratings);

        ASSERT_HINT(server.FindTopDocuments("in").empty(), "Stop words are not ignored");
        ASSERT_HINT(!server.FindTopDocuments("city cat").empty(), "False positive stop words detection");
    }

    {
        SearchServer server("for a");

        server.AddDocument(document_id, "work for a salary", DocumentStatus::kActual, ratings);

        ASSERT_HINT(server.FindTopDocuments("a").empty(), "Stop words are not ignored");
        ASSERT_HINT(!server.FindTopDocuments("work with salary").empty(), "False positive stop words detection");
    }
}

void TestMinusWords() {
    const int document_id = 42;
    const std::vector<int> ratings = { 1 };

    {
        SearchServer server("");

        server.AddDocument(document_id, "cat in the city", DocumentStatus::kActual, ratings);
        server.AddDocument(document_id + 1, "dog in the park", DocumentStatus::kActual, ratings);

        {
            const auto& result = server.FindTopDocuments("in the -dog");

            ASSERT_EQUAL_HINT(result.size(), 1, "Minus words are not ignored");
            ASSERT_EQUAL_HINT(result[0].id, document_id, "Minus words are not ignored, query returned wrong document");
        }

        {
            ASSERT_HINT(server.FindTopDocuments("the -cat -dog").empty(), "Minus words are not ignored");
        }
    }
}

void TestDocumentMatching() {
    const int document_id = 42;

    {
        SearchServer server("");

        server.AddDocument(document_id, "cat in the city", DocumentStatus::kActual, { 1 });

        try
        {
            server.MatchDocument("c\xat", document_id);
            ASSERT_HINT(false, "MatchDocument must not allow illegal characters");
        }
        catch (const std::exception&) {}

        try
        {
            server.MatchDocument("cat --in", document_id);
            ASSERT_HINT(false, "MatchDocument must not allow minus word with second minus");
        }
        catch (const std::exception&) {}

        try
        {
            server.MatchDocument("cat -", document_id);
            ASSERT_HINT(false, "MatchDocument must not allow empty minus words");
        }
        catch (const std::exception&) {}
    }

    {
        SearchServer server("");

        server.AddDocument(document_id, "cat in the city", DocumentStatus::kActual, { 1 });
        {
            const auto& [matched_words, document_status] = server.MatchDocument("cat city", document_id);

            ASSERT_EQUAL_HINT(matched_words.size(), 2, "MatchDocument doesn't match all words in the document");
            ASSERT_EQUAL_HINT(matched_words[0], "cat", "MatchDocument doesn't match some words");
            ASSERT_EQUAL_HINT(matched_words[1], "city", "MatchDocument doesn't match some words");

            ASSERT_EQUAL_HINT(document_status, DocumentStatus::kActual, "Document has wrong status");
        }

        {
            const auto& [matched_words, unused] = server.MatchDocument("cat -city", document_id);

            ASSERT_HINT(matched_words.empty(), "Minus words are not ignored in MatchDocument");
        }
    }

    {
        SearchServer server("");

        server.AddDocument(document_id, "cat in the city", DocumentStatus::kActual, { 1 });
        {
            const auto& [matched_words, document_status] = server.MatchDocument(
                std::execution::par, "cat city", document_id);

            ASSERT_EQUAL_HINT(matched_words.size(), 2, "MatchDocument doesn't match all words in the document");
            ASSERT_EQUAL_HINT(matched_words[0], "cat", "MatchDocument doesn't match some words");
            ASSERT_EQUAL_HINT(matched_words[1], "city", "MatchDocument doesn't match some words");

            ASSERT_EQUAL_HINT(document_status, DocumentStatus::kActual, "Document has wrong status");
        }

        {
            const auto& [matched_words, unused] = server.MatchDocument(std::execution::par, "cat -city", document_id);

            ASSERT_HINT(matched_words.empty(), "Minus words are not ignored in MatchDocument");
        }
    }

    {
        SearchServer server("");

        server.AddDocument(document_id, "dog in the park", DocumentStatus::kActual, { 1 });
        {
            const auto& [matched_words, document_status] = server.MatchDocument("dog in", document_id);

            ASSERT_EQUAL_HINT(matched_words.size(), 2, "MatchDocument doesn't match all words in the document");
            ASSERT_EQUAL_HINT(matched_words[0], "dog", "MatchDocument doesn't match some words");
            ASSERT_EQUAL_HINT(matched_words[1], "in", "MatchDocument doesn't match some words");

            ASSERT_EQUAL_HINT(document_status, DocumentStatus::kActual, "Document has wrong status");
        }

        {
            const auto& [matched_words, unused] = server.MatchDocument("dog -the", document_id);

            ASSERT_HINT(matched_words.empty(), "Minus words are not ignored in MatchDocument");
        }
    }
}

void TestDocumentSorting() {
    const int document_id = 42;
    const std::vector<int> ratings = { 1 };

    {
        SearchServer server("");

        server.AddDocument(document_id, "The cat in the city", DocumentStatus::kActual, ratings);
        server.AddDocument(document_id + 1, "The cat in the", DocumentStatus::kActual, ratings);
        server.AddDocument(document_id + 2, "The cat in", DocumentStatus::kActual, ratings);
        server.AddDocument(document_id + 3, "The", DocumentStatus::kActual, ratings);

        int id = 0;
        const std::vector<std::string> words = {
            "generation", "bake", "quarrel", "ferry", "biscuit",
            "table", "bother", "guideline", "duty", "first",
        };

        for (auto word : words) {
            server.AddDocument(++id, word, DocumentStatus::kActual, ratings);
        }

        {
            const auto& result = server.FindTopDocuments("cat");

            ASSERT_EQUAL_HINT(result.size(), 3, "FindTopDocument incorrect amount of documents");
            constexpr double document_relevance_inaccuracy = 1e-6;

            auto sort_by_relevance = [document_relevance_inaccuracy](const Document& left, const Document& right) {
                return std::abs(left.relevance - right.relevance) < document_relevance_inaccuracy;
            };

            ASSERT_HINT(std::is_sorted(std::begin(result), std::end(result), sort_by_relevance),
                "Documents are not sorted correctly");
        }
    }
}

void TestDocumentRatingCalculation() {
    auto calculate_average_rating = [](const std::vector<int>& input) {
        int correct = 0;
        for (auto number : input) {
            correct += number;
        }
        correct /= static_cast<int>(input.size());
        return correct;
    };

    {
        const std::vector<int> ratings = { 1, 10, 28, 60, 11, 11, 12321 };

        SearchServer server("");

        server.AddDocument(42, "cat in the city", DocumentStatus::kActual, ratings);

        {
            const auto& result = server.FindTopDocuments("cat");

            ASSERT_EQUAL_HINT(result[0].rating, calculate_average_rating(ratings), "Incorrect rating calculation");
        }
    }

    {
        const std::vector<int> ratings = { 545, 136, 548, 508, 797, 21005, 245 };

        SearchServer server("");

        server.AddDocument(42, "cat in the city", DocumentStatus::kActual, ratings);

        {
            const auto& result = server.FindTopDocuments("cat");

            ASSERT_EQUAL_HINT(result[0].rating, calculate_average_rating(ratings), "Incorrect rating calculation");
        }
    }
}

void TestDocumentFilteringUsingPredicate() {
    const int document_id = 42;
    const std::vector<int> ratings = { 1 };

    {
        SearchServer server("");

        server.AddDocument(document_id, "The cat in the city", DocumentStatus::kActual, ratings);
        server.AddDocument(document_id + 1, "The cat in the", DocumentStatus::kActual, ratings);
        server.AddDocument(document_id + 2, "The cat in", DocumentStatus::kActual, ratings);
        server.AddDocument(document_id + 3, "The", DocumentStatus::kActual, ratings);

        {
            auto get_every_second_document = [](int document_id, DocumentStatus, int) {
                return document_id % 2 == 0;
            };

            const auto& result = server.FindTopDocuments("cat", get_every_second_document);

            ASSERT_EQUAL_HINT(result.size(), 2, "Function filter is not applied correctly");

            ASSERT_EQUAL_HINT(result[0].id, document_id + 2,
                "Function filter is not applied correctly, wrong document");

            ASSERT_EQUAL_HINT(result[1].id, document_id, "Fuction filter is not applied correctly, wrong document");
        }
    }

    {
        SearchServer server("");

        server.AddDocument(document_id, "The dog in the park", DocumentStatus::kActual, ratings);
        server.AddDocument(document_id + 1, "The dog in the", DocumentStatus::kActual, ratings);
        server.AddDocument(document_id + 2, "The dog in", DocumentStatus::kActual, ratings);
        server.AddDocument(document_id + 3, "The", DocumentStatus::kActual, ratings);

        {
            auto get_specific_document = [document_id](int current_document_id, DocumentStatus, int) {
                return current_document_id == document_id;
            };

            const auto& result = server.FindTopDocuments("dog", get_specific_document);

            ASSERT_EQUAL_HINT(result.size(), 1, "Function filter is not applied correctly");

            ASSERT_EQUAL_HINT(result[0].id, document_id, "Fuction filter is not applied correctly, wrong document");
        }
    }
}

void TestDocumentFilteringUsingStatus() {
    const int document_id = 42;
    const std::vector<int> ratings = { 1 };

    {
        SearchServer server("");

        server.AddDocument(document_id, "The cat in the city", DocumentStatus::kActual, ratings);
        server.AddDocument(document_id + 1, "The cat in the", DocumentStatus::kBanned, ratings);
        server.AddDocument(document_id + 2, "The cat in", DocumentStatus::kIrrelevant, ratings);
        server.AddDocument(document_id + 3, "The cat", DocumentStatus::kRemoved, ratings);

        {
            const auto& result = server.FindTopDocuments("cat", DocumentStatus::kRemoved);

            ASSERT_EQUAL_HINT(result.size(), 1, "Status filter is not applied correctly");

            ASSERT_EQUAL_HINT(result[0].id, (document_id + 3),
                "Status filter is not applied correctly, wrong document");
        }
    }

    {
        SearchServer server("");

        server.AddDocument(document_id, "The dog in the park", DocumentStatus::kActual, ratings);
        server.AddDocument(document_id + 1, "The dog in the", DocumentStatus::kBanned, ratings);
        server.AddDocument(document_id + 2, "The dog in", DocumentStatus::kBanned, ratings);
        server.AddDocument(document_id + 3, "The cat", DocumentStatus::kRemoved, ratings);

        {
            const auto& result = server.FindTopDocuments("dog", DocumentStatus::kBanned);

            ASSERT_EQUAL_HINT(result.size(), 2, "Status filter is not applied correctly");
            ASSERT_EQUAL_HINT(result[0].id, (document_id + 2),
                "Status filter is not applied correctly, wrong document");

            ASSERT_EQUAL_HINT(result[1].id, (document_id + 1),
                "Status filter is not applied correctly, wrong document");
        }
    }
}

void TestDocumentRelevance() {
    const int document_id = 42;
    const std::vector<int> ratings = { 1 };

    {
        SearchServer server("");

        server.AddDocument(document_id, "The cat in the city", DocumentStatus::kActual, ratings);
        server.AddDocument(document_id + 1, "The cat in the", DocumentStatus::kActual, ratings);
        server.AddDocument(document_id + 2, "The cat in", DocumentStatus::kActual, ratings);
        server.AddDocument(document_id + 3, "The", DocumentStatus::kActual, ratings);

        {
            const auto& result = server.FindTopDocuments("cat");

            ASSERT_EQUAL_HINT(result.size(), 3, "Incorrect amount of returned documents");

            const double idf = log(4.0 / 3.0);

            ASSERT_EQUAL_HINT(result[0].relevance, (1.0 / 3.0) * idf, "Relevance is not calculated correctly");

            ASSERT_EQUAL_HINT(result[1].relevance, (1.0 / 4.0) * idf, "Relevance is not calculated correctly");

            ASSERT_EQUAL_HINT(result[2].relevance, (1.0 / 5.0) * idf, "Relevance is not calculated correctly");
        }
    }

    {
        SearchServer server("");

        server.AddDocument(document_id, "the severity of this issue is high", DocumentStatus::kActual, ratings);
        server.AddDocument(document_id + 1, "the severity is high", DocumentStatus::kActual, ratings);
        server.AddDocument(document_id + 2, "severity is mostly low", DocumentStatus::kActual, ratings);
        server.AddDocument(document_id + 3, "cats just cats", DocumentStatus::kActual, ratings);

        {
            const auto& result = server.FindTopDocuments("severity high");

            ASSERT_EQUAL_HINT(result.size(), 3, "Incorrect amount of returned documents");

            const double severity_idf = log(4.0 / 3.0);
            const double high_idf = log(4.0 / 2.0);

            ASSERT_EQUAL_HINT(result[0].relevance, (1.0 / 4.0) * severity_idf + (1.0 / 4.0) * high_idf,
                "Relevance is not calculated correctly");

            ASSERT_EQUAL_HINT(result[1].relevance, (1.0 / 7.0) * severity_idf + (1.0 / 7.0) * high_idf,
                "Relevance is not calculated correctly");

            ASSERT_EQUAL_HINT(result[2].relevance, (1.0 / 4.0) * severity_idf,
                "Relevance is not calculated correctly");
        }
    }
}

void TestSearchServer() {
    RUN_TEST(TestServerConstructor);
    RUN_TEST(TestDocumentAdding);
    RUN_TEST(TestDocumentCount);
    RUN_TEST(TestStopWords);
    RUN_TEST(TestMinusWords);
    RUN_TEST(TestDocumentMatching);
    RUN_TEST(TestDocumentSorting);
    RUN_TEST(TestDocumentRatingCalculation);
    RUN_TEST(TestDocumentFilteringUsingPredicate);
    RUN_TEST(TestDocumentFilteringUsingStatus);
    RUN_TEST(TestDocumentRelevance);
}


#endif // _DEBUG