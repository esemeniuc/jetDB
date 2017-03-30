#include <iostream>
#include <pqxx/pqxx>

int main(int argc, char** argv){
  pqxx::connection c("dbname=postgres user=postgres password= hostname=localhost");
  pqxx::work txn(c);

  pqxx::result r = txn.exec(
    "SELECT pid, Model "
    "FROM Plane "
    "WHERE Model =" + txn.quote(argv[1]));

  if (r.size() != 1)
  {
    std::cerr
      << "Expected 1 plane with name " << argv[1] << ", "
      << "but found " << r.size() << std::endl;
    return 1;
  }

	std::cout << "Found " << r.size() << "   planes:" << std::endl;
	for (auto row: r)
	{
		std::cout << row[0].c_str() << row[1].c_str() << std::endl;
	}
//  int employee_id = r[0][0].as<int>();
//  std::cout << "Updating employee #" << employee_id << std::endl;
//
//  txn.exec(
//    "UPDATE EMPLOYEE "
//    "SET salary = salary + 1 "
//    "WHERE id = " + txn.quote(employee_id));
//
//  txn.commit();
}
