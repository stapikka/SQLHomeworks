#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/backend/Sqlite3.h>
#include <string>
#include <memory>

namespace dbo = Wt::Dbo;

class Publisher {
public:
    std::string name;

    template<class Action>
    void persist(Action& a) {
        dbo::field(a, name, "name");
    }
};

class Book {
public:
    std::string title;
    dbo::ptr<Publisher> publisher;

    template<class Action>
    void persist(Action& a) {
        dbo::field(a, title, "title");
        dbo::belongsTo(a, publisher, "publisher");
    }
};

class Shop {
public:
    std::string name;

    template<class Action>
    void persist(Action& a) {
        dbo::field(a, name, "name");
    }
};

class Stock {
public:
    dbo::ptr<Book> book;
    dbo::ptr<Shop> shop;
    int count;

    template<class Action>
    void persist(Action& a) {
        dbo::belongsTo(a, book, "book");
        dbo::belongsTo(a, shop, "shop");
        dbo::field(a, count, "count");
    }
};

class Sale {
public:
    double price;
    std::string date_sale;
    dbo::ptr<Stock> stock;
    int count;

    template<class Action>
    void persist(Action& a) {
        dbo::field(a, price, "price");
        dbo::field(a, date_sale, "date_sale");
        dbo::belongsTo(a, stock, "stock");
        dbo::field(a, count, "count");
    }
};

class DatabaseManager {
private:
    std::unique_ptr<dbo::backend::Sqlite3> sqlite3_;
    dbo::Session session_;

public:
    DatabaseManager(const std::string& dbPath) {
        sqlite3_ = std::make_unique<dbo::backend::Sqlite3>(dbPath);
        session_.setConnection(std::move(sqlite3_));

        session_.mapClass<Publisher>("publisher");
        session_.mapClass<Book>("book");
        session_.mapClass<Shop>("shop");
        session_.mapClass<Stock>("stock");
        session_.mapClass<Sale>("sale");

        createTables();
    }

    void createTables() {
        try {
            session_.createTables();
        }
        catch (std::exception& e) {
        }
    }

    dbo::Session& session() { return session_; }
};

int main() {
    try {
        DatabaseManager dbManager("bookstore.db");
        dbo::Session& session = dbManager.session();

        dbo::Transaction transaction(session);

        dbo::ptr<Publisher> publisher = session.addNew<Publisher>();
        publisher.modify()->name = "Test Publisher";

        dbo::ptr<Book> book = session.addNew<Book>();
        book.modify()->title = "Test Book";
        book.modify()->publisher = publisher;

        dbo::ptr<Shop> shop = session.addNew<Shop>();
        shop.modify()->name = "Test Shop";

        dbo::ptr<Stock> stock = session.addNew<Stock>();
        stock.modify()->book = book;
        stock.modify()->shop = shop;
        stock.modify()->count = 10;

        dbo::ptr<Sale> sale = session.addNew<Sale>();
        sale.modify()->price = 29.99;
        sale.modify()->date_sale = "2024-01-15";
        sale.modify()->stock = stock;
        sale.modify()->count = 2;

        transaction.commit();

    }
    catch (std::exception& e) {
        return 1;
    }

    return 0;
}