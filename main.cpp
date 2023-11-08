#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <memory>
#include <algorithm>
#include <execution>
#include <iomanip>
#include <iterator>

enum class RequestType
{
    USER_READ,
    USER_STAT
};

class Request
{
public:
    explicit Request(RequestType type, int id, int position = 0)
        : type_(type), user_id_(id), read_position_(position){};
    RequestType GetType()
    {
        return type_;
    }
    int GetId()
    {
        return user_id_;
    }
    int GetPosition()
    {
        return read_position_;
    }

private:
    const RequestType type_;
    const int user_id_;
    const int read_position_;
};

class Book
{
public:
    explicit Book(const size_t number_of_pages)
    {
        reads_statistic.resize(number_of_pages + 1);
    };

    std::optional<double> RequestHandler(std::unique_ptr<Request> request)
    {
        switch (request->GetType())
        {
        case RequestType::USER_READ:
            ProcessReadRequest(request->GetId(), request->GetPosition());
            return std::nullopt;
            break;
        case RequestType::USER_STAT:
            return ProcessStatRequest(request->GetId());
            break;
        }
        return std::nullopt;
    }

private:
    void ProcessReadRequest(int reader_id, int position)
    {
        size_t first_page_shift = user_statistic.count(reader_id) ? user_statistic.at(reader_id) + 1 : 0;
        user_statistic[reader_id] = position;
        auto start_page = reads_statistic.begin();
        std::advance(start_page, first_page_shift);
        auto last_page = reads_statistic.begin();
        std::advance(last_page, position + 1);
        std::for_each(std::execution::par, start_page, last_page,
                      [](auto &page_reads)
                      {
                          ++page_reads;
                      });
    }
    double ProcessStatRequest(int reader_id)
    {
        if (user_statistic.count(reader_id))
        {
            if (user_statistic.size() == 1)
            {
                return 1;
            }
            int current_reader_page = user_statistic.at(reader_id);
            int readers_on_same_page = reads_statistic[current_reader_page] - 1;
            int readers_except_current = user_statistic.size() - 1;
            return 1. - static_cast<double>(readers_on_same_page) / static_cast<double>(readers_except_current);
        }
        else
        {
            return 0;
        }
    }
    std::unordered_map<int, int> user_statistic;
    std::vector<int> reads_statistic;
};

std::unique_ptr<Request> ReadRequest()
{
    using namespace std::literals;
    std::string in = "";
    std::getline(std::cin, in);
    if (in.find("READ"s) != in.npos)
    {
        const size_t first_digit_position = 5;
        in = in.substr(first_digit_position);
        size_t delimeter_position = in.find(" "s);
        int reader_id = std::stoi(in.substr(0, delimeter_position));
        int position = std::stoi(in.substr(delimeter_position));
        return std::move(std::make_unique<Request>(Request{RequestType::USER_READ, reader_id, position}));
    }
    else
    {
        const size_t first_digit_position = 6;
        int reader_id = std::stoi(in.substr(first_digit_position));
        return std::move(std::make_unique<Request>(Request{RequestType::USER_STAT, reader_id}));
    }
}

int main()
{
    const size_t book_pages = 1000;
    Book e_book(book_pages);
    int request_number;
    (std::cin >> request_number).get();
    while (request_number > 0)
    {
        auto result = e_book.RequestHandler(std::move(ReadRequest()));
        if (result.has_value())
        {
            std::cout << std::setprecision(6) << result.value() << std::endl;
        }
        --request_number;
    }
}