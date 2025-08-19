#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/backend/Postgres.h>
#include <iostream>
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
    std::unique_ptr<dbo::backend::Postgres> postgres_;
    dbo::Session session_;

public:
    DatabaseManager(const std::string& connStr) {
        postgres_ = std::make_unique<dbo::backend::Postgres>(connStr);
        session_.setConnection(std::move(postgres_));

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

    void insertTestData() {
        dbo::Transaction transaction(session_);

        dbo::ptr<Publisher> pub1 = session_.addNew<Publisher>();
        pub1.modify()->name = "Penguin Books";

        dbo::ptr<Publisher> pub2 = session_.addNew<Publisher>();
        pub2.modify()->name = "HarperCollins";

        dbo::ptr<Publisher> pub3 = session_.addNew<Publisher>();
        pub3.modify()->name = "Random House";

        dbo::ptr<Book> book1 = session_.addNew<Book>();
        book1.modify()->title = "The Great Gatsby";
        book1.modify()->publisher = pub1;

        dbo::ptr<Book> book2 = session_.addNew<Book>();
        book2.modify()->title = "To Kill a Mockingbird";
        book2.modify()->publisher = pub2;

        dbo::ptr<Book> book3 = session_.addNew<Book>();
        book3.modify()->title = "1984";
        book3.modify()->publisher = pub1;

        dbo::ptr<Book> book4 = session_.addNew<Book>();
        book4.modify()->title = "Pride and Prejudice";
        book4.modify()->publisher = pub3;

        dbo::ptr<Shop> shop1 = session_.addNew<Shop>();
        shop1.modify()->name = "City Bookstore";

        dbo::ptr<Shop> shop2 = session_.addNew<Shop>();
        shop2.modify()->name = "University Books";

        dbo::ptr<Shop> shop3 = session_.addNew<Shop>();
        shop3.modify()->name = "Mall Bookshop";

        dbo::ptr<Stock> stock1 = session_.addNew<Stock>();
        stock1.modify()->book = book1;
        stock1.modify()->shop = shop1;
        stock1.modify()->count = 15;

        dbo::ptr<Stock> stock2 = session_.addNew<Stock>();
        stock2.modify()->book = book2;
        stock2.modify()->shop = shop1;
        stock2.modify()->count = 8;

        dbo::ptr<Stock> stock3 = session_.addNew<Stock>();
        stock3.modify()->book = book3;
        stock3.modify()->shop = shop2;
        stock3.modify()->count = 12;

        dbo::ptr<Stock> stock4 = session_.addNew<Stock>();
        stock4.modify()->book = book1;
        stock4.modify()->shop = shop3;
        stock4.modify()->count = 5;

        dbo::ptr<Stock> stock5 = session_.addNew<Stock>();
        stock5.modify()->book = book4;
        stock5.modify()->shop = shop2;
        stock5.modify()->count = 10;

        transaction.commit();
    }

    void findShopsByPublisher(const std::string& publisherName) {
        dbo::Transaction transaction(session_);

        typedef dbo::collection<dbo::ptr<Shop>> ShopCollection;

        ShopCollection shops = session_.query<dbo::ptr<Shop>>(
            "SELECT DISTINCT shop FROM stock s "
            "JOIN book b ON s.book_id = b.id "
            "JOIN publisher p ON b.publisher_id = p.id "
            "WHERE p.name = ?")
            .bind(publisherName);

        std::cout << "Shops selling books from publisher '" << publisherName << "':" << std::endl;

        if (shops.size() == 0) {
            std::cout << "No shops found for this publisher." << std::endl;
        }
        else {
            for (const auto& shop : shops) {
                std::cout << "- " << shop->name << std::endl;
            }
        }
    }

    dbo::Session& session() { return session_; }
};

int main() {
    try {
        std::string connStr = "host=localhost port=5432 dbname=bookstore user=postgres password=1012";
        DatabaseManager dbManager(connStr);

        dbManager.insertTestData();

        std::string publisherName;
        std::cout << "Enter publisher name: ";
        std::getline(std::cin, publisherName);

        dbManager.findShopsByPublisher(publisherName);

    }
    catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}