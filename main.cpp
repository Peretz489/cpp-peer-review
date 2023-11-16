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
    USER_STAT,
    BAD_REQUEST
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
    int GetUserId() const
    {
        return user_id_;
    }
    int GetReadPosition() const
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
    explicit Book(const size_t number_of_pages, const size_t number_of_users)
    {
        user_statistic_.reserve(number_of_users + 1);
        reads_statistic_.resize(number_of_pages + 1);
    };

    std::optional<double> RequestHandler(std::unique_ptr<Request> request)
    {
        switch (request->GetType())
        {
        case RequestType::USER_READ:
            ProcessReadRequest(request->GetUserId(), request->GetReadPosition());
            return std::nullopt;
            break;
        case RequestType::USER_STAT:
            return ProcessStatRequest(request->GetUserId());
            break;
        }
        return std::nullopt;
    }

private:
    void ProcessReadRequest(const int reader_id, const int position)
    {
        size_t first_page_shift = user_statistic_.count(reader_id) ? user_statistic_.at(reader_id) + 1 : 0;
        user_statistic_[reader_id] = position;
        auto start_page = reads_statistic_.begin();
        std::advance(start_page, first_page_shift);
        auto last_page = reads_statistic_.begin();
        std::advance(last_page, position + 1);
        std::for_each(std::execution::par, start_page, last_page,
                      [](auto &page_reads)
                      {
                          ++page_reads;
                      });
    }
    double ProcessStatRequest(const int reader_id) const
    {
        if (user_statistic_.count(reader_id))
        {
            if (user_statistic_.size() == 1)
            {
                return 1;
            }
            int current_reader_page = user_statistic_.at(reader_id);
            int readers_on_same_page = reads_statistic_[current_reader_page] - 1;
            int readers_except_current = user_statistic_.size() - 1;
            return 1. - static_cast<double>(readers_on_same_page) / static_cast<double>(readers_except_current);
        }
        else
        {
            return 0;
        }
    }
    std::unordered_map<int, int> user_statistic_;
    std::vector<int> reads_statistic_;
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
        return std::make_unique<Request>(Request{RequestType::USER_READ, reader_id, position});
    }
    else if (in.find("CHEER"s) != in.npos)
    {
        const size_t first_digit_position = 6;
        int reader_id = std::stoi(in.substr(first_digit_position));
        return std::make_unique<Request>(Request{RequestType::USER_STAT, reader_id});
    }
    return std::make_unique<Request>(Request{RequestType::BAD_REQUEST, {}});
}

int main()
{
    const size_t book_pages = 1000;
    const size_t max_user_number = 10000;
    Book e_book(book_pages, max_user_number);
    int request_number;
    (std::cin >> request_number).get();
    while (request_number > 0)
    {
        auto result = e_book.RequestHandler(ReadRequest());
        if (result.has_value())
        {
            std::cout << std::setprecision(6) << result.value() << std::endl;
        }
        --request_number;
    }
}