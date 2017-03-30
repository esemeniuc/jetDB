//
// Created by eric on 3/29/17.
//

#ifndef JETDB_UTILITY_H
#define JETDB_UTILITY_H

#include <pqxx/pqxx>
#include <iostream>

bool isValid(std::string validateString, pqxx::work& txn);

void printResult(const pqxx::result& printResult);

pqxx::result::size_type lookupPrint(std::string lookupString, pqxx::work& txn);

std::string getUserInput(pqxx::work& txn,
						 std::string promptUserString,
						 std::string lookupString,
						 std::string validationString,
						 int numParamsForValidation);

std::string getDate(std::string promptUserString);

#endif //JETDB_UTILITY_H
