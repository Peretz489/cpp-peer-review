struct DbConnectionSettings
{
    string_view db_name;
    int db_connection_timeout;
    bool db_allow_exceptions;
    DBLogLevel db_log_level;
}

class DbRequestHandler
{
public:
    explicit DbRequestHandler(DbConnectionSettings connection_settings)
        : connecttion_settings_(connection_settings)
    {
        connector_ = {connection_settings.db_allow_exceptions, connection_settings.db_log_level};
    };

    bool DbConnect()
    {
        if (db_name.starts_with("tmp."s))
        {
            db = connector.ConnectTmp(db_name, db_connection_timeout);
        }
        else
        {
            db = connector.Connect(db_name, db_connection_timeout);
        }
        connected_ = !db_allow_exceptions && !db.IsOK() ? false : true;
        return connected_;
    }

    bool Is Connected(){
        return connected_;
    }

    bool DbDisconnect(){
        //not implemented yet
        return true;
    }

    vector<Person> LoadPersons(const int min_age, const int max_age, const string_view name_filter) const
    {
        ostringstream query_str;
        query_str << "from Persons "s
                  << "select Name, Age "s
                  << "where Age between "s << min_age << " and "s << max_age << " "s
                  << "and Name like '%"s << db.Quote(name_filter) << "%'"s;
        DBQuery query(query_str.str());

        vector<Person> persons;
        for (auto [name, age] : db.LoadRows<string, int>(query))
        {
            persons.push_back({move(name), age});
        }
        return persons;
    }

private:
    DbConnectionSettings connection_settings_;
    DBConnector connector_;
    DBHandler db_;
    bool connected_= false;
}
