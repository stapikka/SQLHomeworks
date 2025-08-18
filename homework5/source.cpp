#include <iostream>
#include <pqxx/pqxx>

int main() {
    setlocale(LC_ALL, "rus");
    try {
        // Подключение
        pqxx::connection c(
            "host=localhost "
            "port=5432 "
            "dbname=test "
            "user=postgres "
            "password=1012"
        );

        if (!c.is_open()) {
            std::cerr << "Не удалось подключиться к базе данных!" << std::endl;
            return 1;
        }

        // Проверка существования таблицы
        bool table_exists = false;
        {
            pqxx::nontransaction ntx(c);
            table_exists = ntx.query_value<bool>(
                "SELECT EXISTS (SELECT FROM information_schema.tables WHERE table_name = 'Student')");
        }

        if (!table_exists) {
            std::cerr << "Таблица 'Student' не существует!" << std::endl;
            return 1;
        }

         //Выполнение запроса
        pqxx::work tx{ c };
        auto result = tx.exec("SELECT name FROM Student");

        for (const auto& row : result) {
            std::cout << row["name"].as<std::string>() << ".\n";
        }
    }
    catch (const pqxx::broken_connection& e) {
        std::cerr << "Ошибка подключения: " << e.what() << std::endl;
        return 1;
    }
    catch (const pqxx::sql_error& e) {
        std::cerr << "SQL ошибка: " << e.what() << std::endl;
        return 1;
    }
    catch (const std::exception& e) {
        std::cerr << "Общая ошибка: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}