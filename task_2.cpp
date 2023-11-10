#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <string_view>
#include <vector>
#include <execution>
#include <cassert>

// uncomment to run tests
//#define DEBUG

using namespace std;

class Domain
{

public:
    Domain() = default;
    explicit Domain(const std::string &domain_name)
        : reversed_domain_(domain_name.rbegin(), domain_name.rend()){};

    bool operator==(const Domain &rhs)
    {
        return this->GetName().size() <= rhs.GetName().size()
                   ? rhs.IsSubdomain(*this)
                   : this->IsSubdomain(rhs);
    }

    bool IsSubdomain(const Domain &other) const
    {
        if (this->reversed_domain_.size() < other.reversed_domain_.size())
        {
            return false;
        }
        else
        {
            auto last_symbol_ptr = reversed_domain_.begin();
            std::advance(last_symbol_ptr, other.reversed_domain_.size());
            return std::equal(std::execution::par, reversed_domain_.begin(), last_symbol_ptr, other.reversed_domain_.begin(), other.reversed_domain_.end());
        }
    }

    const std::string GetName() const
    {
        return reversed_domain_;
    }

private:
    std::string reversed_domain_;
};

class DomainChecker
{
public:
    template <typename It>
    DomainChecker(It start, It end)
        : forbidden_list_{start, end}
    {
        std::sort(std::execution::par, forbidden_list_.begin(), forbidden_list_.end(), [](const auto &lhs, const auto &rhs)
                  { return lhs.GetName() < rhs.GetName(); });
        auto last_unique = std::unique(forbidden_list_.begin(), forbidden_list_.end());
        size_t unique_items = std::distance(forbidden_list_.begin(), last_unique);
        forbidden_list_.resize(unique_items);
    }

    bool IsForbidden(const Domain &domain) const
    {
        if (domain.GetName().size() == 1)
        {
            return true;
        }
        auto position = std::upper_bound(forbidden_list_.begin(), forbidden_list_.end(), domain,
                                         [](const Domain &lhs, const Domain &rhs)
                                         {
                                             const std::string &domain1 = lhs.GetName();
                                             const std::string &domain2 = rhs.GetName();
                                             return std::lexicographical_compare(domain1.begin(), domain1.end(),
                                                                                 domain2.begin(), domain2.end());
                                         });
        if (position != forbidden_list_.begin())
        {
            return domain.IsSubdomain(*std::prev(position)) ? true : false;
        }
        else
        {
            return false;
        }
    }

private:
    std::vector<Domain> forbidden_list_;
};

std::vector<Domain> ReadDomains(std::istream &in, size_t num)
{
    std::vector<Domain> out;
    std::string in_str = "";
    while (num > 0)
    {
        std::getline(in, in_str);
        out.emplace_back(Domain{"."s + in_str});
        --num;
    }
    return out;
}

template <typename Number>
Number ReadNumberOnLine(istream &input)
{
    string line;
    getline(input, line);

    Number num;
    std::istringstream(line) >> num;

    return num;
}

std::stringstream &TestForbidden(std::stringstream &out)
{
    out << "ru"s
        << "\n";
    out << "test.ru"s
        << "\n";
    out << "net.qu"s
        << "\n";
    out << "pp.net.du"s
        << "\n";

    return out;
}

std::stringstream &Test1(std::stringstream &out)
{
    out << ""s
        << "\n";
    out << "ru"s
        << "\n";
    out << "test.ru"s
        << "\n";
    out << "pp.net.qu"s
        << "\n";
    out << "aa.pp.net.du"s
        << "\n";
    return out;
}

std::stringstream &Test2(std::stringstream &out)
{
    out << ".u"s
        << "\n";
    out << "net.du"s
        << "\n";
    out << "qu"s
        << "\n";
    out << "pp.not.qu"s
        << "\n";
    out << "aa.pi.net.du"s
        << "\n";
    return out;
}

void RunTests()
{
    //-----domain checker init------
    std::stringstream test_stream;
    const std::vector<Domain> forbidden_domains = ReadDomains(TestForbidden(test_stream), 6);
    DomainChecker checker(forbidden_domains.begin(), forbidden_domains.end());
    test_stream.str() = ""s;
    test_stream.clear();

    //--------bad domains test--------
    for (const Domain &domain : ReadDomains(Test1(test_stream), 5))
    {
        assert(checker.IsForbidden(domain));
    }
    test_stream.str() = ""s;
    test_stream.clear();
    std::cout << "bad domains test passed" << std::endl;

    //--------good domains test---------
    for (const Domain &domain : ReadDomains(Test2(test_stream), 4))
    {
        assert(!checker.IsForbidden(domain));
    }
    std::cout << "good domains test passed" << std::endl;
}

int main()
{
#ifdef DEBUG
    RunTests();
#endif

    const std::vector<Domain> forbidden_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    DomainChecker checker(forbidden_domains.begin(), forbidden_domains.end());

    const std::vector<Domain> test_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    for (const Domain &domain : test_domains)
    {
        cout << (checker.IsForbidden(domain) ? "Bad"sv : "Good"sv) << endl;
    }
}